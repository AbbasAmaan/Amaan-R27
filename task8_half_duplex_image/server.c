#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_FILENAME 255

// Send exactly 'size' bytes
int send_all(int socket, const void *data, size_t size)
{
    size_t total = 0;

    while (total < size)
    {
        ssize_t sent = send(socket,
                            (const char *)data + total,
                            size - total,
                            0);

        if (sent <= 0)
            return -1;

        total += sent;
    }

    return 0;
}

// Receive exactly 'size' bytes
int recv_all(int socket, void *data, size_t size)
{
    size_t total = 0;

    while (total < size)
    {
        ssize_t received = recv(socket,
                                (char *)data + total,
                                size - total,
                                0);

        if (received <= 0)
            return -1;

        total += received;
    }

    return 0;
}

// Send an image
int send_image(int socket)
{
    char path[BUFFER_SIZE];

    printf("\n=================================\n");
    printf("YOUR TURN - SEND IMAGE\n");
    printf("=================================\n");
    printf("Enter image path (or 'exit'): ");

    if (fgets(path, sizeof(path), stdin) == NULL)
        return 1;

    path[strcspn(path, "\n")] = '\0';

    // Exit
    if (strcmp(path, "exit") == 0)
    {
        char command = 'E';

        if (send_all(socket, &command, sizeof(command)) < 0)
            return -1;

        return 1;
    }

    // Open image
    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        printf("ERROR: Could not open '%s'\n", path);
        return 0;
    }

    // Get file size
    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return -1;
    }

    long file_size = ftell(file);

    if (file_size < 0)
    {
        fclose(file);
        return -1;
    }

    rewind(file);

    // Get filename from path
    char *filename = strrchr(path, '/');

    if (filename != NULL)
        filename++;
    else
        filename = path;

    size_t filename_length = strlen(filename);

    if (filename_length == 0 || filename_length > MAX_FILENAME)
    {
        printf("ERROR: Filename is invalid or too long.\n");
        fclose(file);
        return 0;
    }

    // Tell receiver an image is coming
    char command = 'I';

    if (send_all(socket, &command, sizeof(command)) < 0)
    {
        fclose(file);
        return -1;
    }

    // Send filename length
    uint32_t filename_length_net =
        htonl((uint32_t)filename_length);

    if (send_all(socket,
                 &filename_length_net,
                 sizeof(filename_length_net)) < 0)
    {
        fclose(file);
        return -1;
    }

    // Send filename
    if (send_all(socket,
                 filename,
                 filename_length) < 0)
    {
        fclose(file);
        return -1;
    }

    // Send file size
    int64_t file_size_net = (int64_t)file_size;

    if (send_all(socket,
                 &file_size_net,
                 sizeof(file_size_net)) < 0)
    {
        fclose(file);
        return -1;
    }

    printf("\nSending: %s\n", filename);
    printf("Size: %ld bytes\n", file_size);

    // Send image data
    char buffer[BUFFER_SIZE];
    long total_sent = 0;

    while (1)
    {
        size_t bytes_read =
            fread(buffer, 1, BUFFER_SIZE, file);

        if (bytes_read == 0)
            break;

        if (send_all(socket, buffer, bytes_read) < 0)
        {
            printf("\nERROR: Failed to send image.\n");
            fclose(file);
            return -1;
        }

        total_sent += bytes_read;

        printf("\rProgress: %ld / %ld bytes",
               total_sent,
               file_size);

        fflush(stdout);
    }

    fclose(file);

    printf("\nImage sent successfully!\n");

    return 0;
}

// Receive an image
int receive_image(int socket)
{
    char command;

    // Receive command
    if (recv_all(socket, &command, sizeof(command)) < 0)
        return -1;

    // Client wants to exit
    if (command == 'E')
    {
        printf("\nClient ended the connection.\n");
        return 1;
    }

    if (command != 'I')
    {
        printf("\nERROR: Invalid command received.\n");
        return -1;
    }

    // Receive filename length
    uint32_t filename_length_net;

    if (recv_all(socket,
                 &filename_length_net,
                 sizeof(filename_length_net)) < 0)
        return -1;

    uint32_t filename_length =
        ntohl(filename_length_net);

    if (filename_length == 0 ||
        filename_length > MAX_FILENAME)
    {
        printf("\nERROR: Invalid filename length.\n");
        return -1;
    }

    // Receive filename
    char filename[MAX_FILENAME + 1];

    if (recv_all(socket,
                 filename,
                 filename_length) < 0)
        return -1;

    filename[filename_length] = '\0';

    // Receive file size
    int64_t file_size;

    if (recv_all(socket,
                 &file_size,
                 sizeof(file_size)) < 0)
        return -1;

    if (file_size < 0)
    {
        printf("\nERROR: Invalid file size.\n");
        return -1;
    }

    printf("\n=================================\n");
    printf("RECEIVING IMAGE\n");
    printf("=================================\n");

    printf("Filename: %s\n", filename);
    printf("Size: %ld bytes\n", (long)file_size);

    // Create output filename
    char output_name[BUFFER_SIZE];

    snprintf(output_name,
             sizeof(output_name),
             "received_%.*s",
             MAX_FILENAME,
             filename);

    // Open output file
    FILE *file = fopen(output_name, "wb");

    if (file == NULL)
    {
        printf("ERROR: Could not create '%s'\n",
               output_name);
        return -1;
    }

    // Receive image data
    char buffer[BUFFER_SIZE];
    int64_t total_received = 0;

    while (total_received < file_size)
    {
        int64_t remaining =
            file_size - total_received;

        size_t bytes_to_receive =
            remaining < BUFFER_SIZE
                ? (size_t)remaining
                : BUFFER_SIZE;

        ssize_t bytes_received =
            recv(socket,
                 buffer,
                 bytes_to_receive,
                 0);

        if (bytes_received <= 0)
        {
            printf("\nERROR: Connection lost while receiving.\n");
            fclose(file);
            return -1;
        }

        fwrite(buffer,
               1,
               bytes_received,
               file);

        total_received += bytes_received;

        printf("\rProgress: %ld / %ld bytes",
               (long)total_received,
               (long)file_size);

        fflush(stdout);
    }

    fclose(file);

    printf("\nImage received successfully!\n");
    printf("Saved as: %s\n", output_name);

    return 0;
}

int main()
{
    int server_socket;
    int client_socket;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    socklen_t client_size =
        sizeof(client_addr);

    // Create socket
    server_socket =
        socket(AF_INET,
               SOCK_STREAM,
               0);

    if (server_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Allow port reuse
    int option = 1;

    setsockopt(server_socket,
               SOL_SOCKET,
               SO_REUSEADDR,
               &option,
               sizeof(option));

    // Clear address structure
    memset(&server_addr,
           0,
           sizeof(server_addr));

    // Server address
    server_addr.sin_family = AF_INET;

    server_addr.sin_addr.s_addr =
        INADDR_ANY;

    server_addr.sin_port =
        htons(PORT);

    // Bind
    if (bind(server_socket,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    // Listen
    if (listen(server_socket, 1) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }

    printf("=================================\n");
    printf("     HALF-DUPLEX IMAGE SERVER\n");
    printf("=================================\n");

    printf("Waiting for client on port %d...\n",
           PORT);

    // Accept client
    client_socket =
        accept(server_socket,
               (struct sockaddr *)&client_addr,
               &client_size);

    if (client_socket < 0)
    {
        perror("Accept failed");
        close(server_socket);
        return 1;
    }

    printf("\nClient connected!\n");

    // Half-duplex communication
    while (1)
    {
        // SERVER SENDS
        int result =
            send_image(client_socket);

        if (result != 0)
            break;

        // SERVER RECEIVES
        printf("\nWaiting for client's image...\n");

        result =
            receive_image(client_socket);

        if (result != 0)
            break;
    }

    // Close sockets
    close(client_socket);
    close(server_socket);

    printf("\nConnection closed.\n");

    return 0;
}

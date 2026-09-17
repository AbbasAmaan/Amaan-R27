#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

int client_socket;

void *receive_messages(void *arg)
{
    char buffer[1024];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        int n = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (n <= 0)
        {
            printf("\nServer disconnected.\n");
            exit(0);
        }

        printf("\nServer: %s", buffer);
        printf("Client: ");
        fflush(stdout);
    }

    return NULL;
}

int main()
{
    struct sockaddr_in server_address;
    pthread_t thread;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5001);

    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    connect(client_socket, (struct sockaddr *)&server_address,
            sizeof(server_address));

    printf("Connected to server!\n");

    pthread_create(&thread, NULL, receive_messages, NULL);

    char buffer[1024];

    while (1)
    {
        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);

        send(client_socket, buffer, strlen(buffer), 0);
    }

    return 0;
}

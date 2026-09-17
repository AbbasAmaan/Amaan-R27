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
            printf("\nClient disconnected.\n");
            exit(0);
        }

        printf("\nClient: %s", buffer);
        printf("Server: ");
        fflush(stdout);
    }

    return NULL;
}

int main()
{
    int server_socket;
    struct sockaddr_in server_address;
    pthread_t thread;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(5001);

    bind(server_socket, (struct sockaddr *)&server_address,
         sizeof(server_address));

    listen(server_socket, 1);

    printf("Server waiting for client...\n");

    client_socket = accept(server_socket, NULL, NULL);

    printf("Client connected!\n");

    pthread_create(&thread, NULL, receive_messages, NULL);

    char buffer[1024];

    while (1)
    {
        printf("Server: ");
        fgets(buffer, sizeof(buffer), stdin);

        send(client_socket, buffer, strlen(buffer), 0);
    }

    return 0;
}

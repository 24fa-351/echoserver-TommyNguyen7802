#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc != 3 || strcmp(argv[1], "-p") != 0)
    {
        printf("Usage: %s -p <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("socket");
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind socket to the specified port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error("bind");
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0)
    {
        error("listen");
    }

    printf("Server listening on port %d\n", port);

    while (1) {
        // Accept a new connection
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0)
        {
            error("accept");
        }

        printf("Client connected\n");

        // Echo received messages back to the client
        ssize_t n;
        while ((n = read(client_fd, buffer, BUFFER_SIZE)) > 0)
        {
            buffer[n] = '\0'; // Null-terminate the received string
            printf("Received: %s", buffer);
            if (write(client_fd, buffer, n) < 0)
            {
                error("write");
            }
        }

        if (n < 0)
        {
            error("read");
        }

        printf("Client disconnected\n");
        close(client_fd);
    }

    close(server_fd);
    return 0;
}

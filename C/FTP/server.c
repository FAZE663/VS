#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 512
#define PORT 8080

void handle_client(int client_sock) {
    char filename[256];
    char buffer[BUFFER_SIZE];
    char error_msg[] = "ERROR: FILE NOT FOUND";
    FILE *fp;
    int count;

    // Receive the filename
    count = recv(client_sock, filename, sizeof(filename) - 1, 0);
    if (count <= 0) {
        perror("Failed to receive filename");
        close(client_sock);
        return;
    }
    filename[count] = '\0';
    printf("Client requested file: %s\n", filename);

    // Open the file
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("File not found");
        send(client_sock, error_msg, strlen(error_msg), 0);
    } else {
        printf("Sending file to client...\n");
        while ((count = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
            if (send(client_sock, buffer, count, 0) == -1) {
                perror("Failed to send file data");
                break;
            }
        }
        fclose(fp);
        printf("File sent successfully\n");
    }

    close(client_sock);
    printf("Client connection closed\n");
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(server_sock);
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_sock, 4) == -1) {
        perror("Listening failed");
        close(server_sock);
        return 1;
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept connections
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock == -1) {
            perror("Accepting connection failed");
            continue;
        }
        printf("Client connected from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        handle_client(client_sock);
    }

    close(server_sock);
    return 0;
}


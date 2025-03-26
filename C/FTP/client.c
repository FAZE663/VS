#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 512
#define PORT 8080

void handle_file_transfer(int sockfd);

int main() {
    int sockfd;
    char filename[256];
    struct sockaddr_in server_addr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection to server failed");
        close(sockfd);
        return 1;
    }

    printf("[INFO] Connected to server on port %d\n", PORT);
    printf("Enter filename to request: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0; // Remove newline character

    // Send filename to server
    if (send(sockfd, filename, strlen(filename), 0) == -1) {
        perror("Failed to send filename");
        close(sockfd);
        return 1;
    }
    printf("Filename sent successfully\n");

    // Handle file transfer
    handle_file_transfer(sockfd);
    close(sockfd);
    return 0;
}

void handle_file_transfer(int sockfd) {
    char buffer[BUFFER_SIZE];
    int count;
    FILE *fp = fopen("output.txt", "wb");

    if (!fp) {
        perror("[ERROR] Failed to create output file");
        return;
    }

    while ((count = recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        if (strncmp(buffer, "ERROR:FILE NOT FOUND", 20) == 0) {
            printf("[ERROR] %s\n", buffer);
            fclose(fp);
            remove("output.txt");
            return;
        }
        fwrite(buffer, 1, count, fp);
    }

    if (count == -1) {
        perror("Error receiving data");
    } else {
        printf("File received successfully. Saved as output.txt\n");
    }

    fclose(fp);
} 


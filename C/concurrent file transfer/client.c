#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char file_buf[512];
    char file_name[256];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        printf("Socket creation failed\n");
        return 0;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8000);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed\n");
        return 0;
    }

    printf("Enter the filename: ");
    scanf("%s", file_name);

    if (send(sock_fd, file_name, strlen(file_name), 0) < 0) {
        printf("Error sending filename\n");
        return 0;
    }

    printf("File request sent\n");

    FILE *fp = fopen("output.txt", "wb");
    if (!fp) {
        printf("Error creating output file\n");
        return 0;
    }

    int bytes;
    while ((bytes = recv(sock_fd, file_buf, sizeof(file_buf), 0)) > 0) {
        if (strncmp(file_buf, "NOFILEFOUND", 11) == 0) {
            printf("Error: File not found on server\n");
            fclose(fp);
            remove("output.txt");
            return 0;
        }
        fwrite(file_buf, 1, bytes, fp);
    }

    if (bytes < 0) {
        printf("Error receiving file data\n");
        return 0;
    }

    printf("File received and saved to output.txt\n");

    fclose(fp);
    close(sock_fd);
    return 0;
}

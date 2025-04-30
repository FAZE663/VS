#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void *client_handler(void *arg) {
    int client_fd = *((int *)arg);
    free(arg);
    char data_buf[512];
    char file_name[256];

    int name_len = recv(client_fd, file_name, sizeof(file_name) - 1, 0);
    if (name_len <= 0) {
        close(client_fd);
        return NULL;
    }

    file_name[name_len] = '\0';
    printf("File requested: %s\n", file_name);

    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        send(client_fd, "NOFILEFOUND", 11, 0);
    } else {
        int bytes_read;
        while ((bytes_read = fread(data_buf, 1, sizeof(data_buf), fp)) > 0) {
            send(client_fd, data_buf, bytes_read, 0);
        }
        fclose(fp);
    }

    close(client_fd);
    printf("Client disconnected\n");
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t thread_id;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("Socket creation failed\n");
        return 0;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8000);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Binding failed\n");
        return 0;
    }

    if (listen(server_fd, 3) < 0) {
        printf("Listening failed\n");
        return 0;
    }

    printf("Server listening on port 8000\n");

    while (1) {
        int *client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (*client_fd < 0) {
            free(client_fd);
            continue;
        }

        printf("New client connected\n");

        if (pthread_create(&thread_id, NULL, client_handler, client_fd) != 0) {
            close(*client_fd);
            free(client_fd);
            continue;
        }

        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}

#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>

struct emailindex{
    char email[50];
    int socket_id;
};
int connected_clients=0;
struct emailindex clientlist[10];


int find_client(char msg[]){
    char email[50];
    char *start = strstr(msg, "RCPT<SP>TO<CRLF>");
    if (start){
        start+=strlen("RCPT<SP>TO<CRLF>");
        char *end = strstr(start,"<CRLF>");
        if (end && (end - start) < sizeof(email)) {
            strncpy(email, start, end - start);
            email[end - start] = '\0';  
            printf("Extracted email: %s\n", email);

            for(int i =0;i<connected_clients;i++){
                if(strcmp(clientlist[i].email,email)==0){
                    return clientlist[i].socket_id;
                }
            }
            return -1;
        } else {
            printf("Email not properly terminated\n");
        }
    } 
    else {
        printf("Pattern not found\n");
    }

    return 0;
    
}

void *handle_client(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[1024];
    char client_email[50] = "";
    char recipient_email[50] = "";
    char message_body[512] = "";

    // Step 1: Register email
    int r1 = recv(sockfd, buffer, sizeof(buffer), 0);
    if (r1 > 0) {
        strcpy(clientlist[connected_clients].email, buffer);
        clientlist[connected_clients].socket_id = sockfd;
        connected_clients++;
    }

    printf("Client registered: %s\n", clientlist[connected_clients - 1].email);
    send(sockfd, "220 Simple SMTP Server Ready\r\n", strlen("220 Simple SMTP Server Ready\r\n"), 0);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int r2 = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (r2 <= 0) {
            printf("Client disconnected\n");
            break;
        }

        buffer[r2] = '\0';
        printf("Received: %s\n", buffer);

        if (strncmp(buffer, "HELO", 4) == 0) {
            send(sockfd, "250 Hello client\r\n", strlen("250 Hello client\r\n"), 0);
        }
        else if (strstr(buffer, "RCPT<SP>TO<CRLF>")) {
            char *start = strstr(buffer, "RCPT<SP>TO<CRLF>") + strlen("RCPT<SP>TO<CRLF>");
            char *end = strstr(start, "<CRLF>");
            if (end && (end - start) < sizeof(recipient_email)) {
                strncpy(recipient_email, start, end - start);
                recipient_email[end - start] = '\0';

                // Recipient is considered valid if they're registered
                int found = 0;
                for (int i = 0; i < connected_clients; i++) {
                    if (strcmp(clientlist[i].email, recipient_email) == 0) {
                        found = 1;
                        break;
                    }
                }

                if (found) {
                    send(sockfd, "250 Recipient OK\r\n", strlen("250 Recipient OK\r\n"), 0);
                } else {
                    send(sockfd, "250 Recipient OK\r\n", strlen("250 Recipient OK\r\n"), 0); // still OK
                }
            } else {
                send(sockfd, "500 Syntax error in RCPT TO\r\n", strlen("500 Syntax error in RCPT TO\r\n"), 0);
            }
        }
        else if (strstr(buffer, "DATA<CRLF>")) {
            char *start = strstr(buffer, "DATA<CRLF>") + strlen("DATA<CRLF>");
            strncpy(message_body, start, sizeof(message_body) - 1);
            message_body[sizeof(message_body) - 1] = '\0';

            printf("Storing message for %s: %s\n", recipient_email, message_body);
            send(sockfd, "250 Message accepted for delivery\r\n", strlen("250 Message accepted for delivery\r\n"), 0);
        }
        else {
            send(sockfd, "500 Command not recognized\r\n", strlen("500 Command not recognized\r\n"), 0);
        }
    }

    close(sockfd);
    pthread_exit(NULL);
}


int main(){


    printf("staring server..\n");
    int sock, *client_sock;
    struct sockaddr_in saddr,caddr;
    

    sock=socket(AF_INET,SOCK_STREAM,0);
    if (sock<0){
        printf("Socket creation failed\n");
        return 0;
    }
    printf("socket creation succesfull\n");
    saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(5000);

    if(bind(sock,(struct sockaddr*)&saddr,sizeof(saddr))<0){
        perror("binding failed\n");
        return 0;
    }
    else{
        printf("binding good");
        if (listen(sock,5)<0){
            printf("listening failed");
            return 0;
        }

        printf("server up and running\n");

        while(1){
            client_sock = malloc(sizeof(int));
            char emailaddr[50];
            memset(emailaddr,0,sizeof(emailaddr));
            int clen=sizeof(caddr);
            *client_sock=accept(sock,(struct sockaddr*)&caddr,&clen);
            if (*client_sock<0){
                perror("connection failed");
                continue;
            }
            else{
                printf("connected to client. client id: %d\n",*client_sock);
                pthread_t tid;
                if (pthread_create(&tid,NULL, handle_client,client_sock)!=0){
                    printf("thread creation failed");

                }
                pthread_detach(tid);

                
            }

        }
        close(sock);

    }

}
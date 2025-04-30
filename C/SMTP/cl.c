#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>
#include<pthread.h>
/*
void *recievemail(void *arg){
    int sock = *(int*)arg;
    char buffer[1024];
    
    while(1){
        if (recv(sock,buffer,sizeof(buffer),0) > 0){
            printf("new mail recieved: %s",buffer);
        }
    }
}
*/


int main(){
    int sock,isock;
    struct sockaddr_in saddr;
    char buffer[1024],mail[50],body[500];

    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        printf("error");
        return -1;
    }
    saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    saddr.sin_port=htons(5000);
    saddr.sin_family=AF_INET;

    isock=connect(sock,(struct sockaddr*)&saddr,sizeof(saddr));
    if (isock<0){
        printf("error connecting to server\n");
        return -1;
    }
    else{
        printf("connected to server\n\n\n");
        printf("-------email login-------\n");
        printf("enter your email: ");
        scanf("%s",buffer);
        int s1=send(sock,buffer,sizeof(buffer),0);
        if (s1<0){
            printf("\nerror: not send");
            return -1;
        }
        else{
            printf("\nregistered in server succesfully\n");
            while (1)
            {
                // After registering:
                recv(sock, buffer, sizeof(buffer), 0);
                printf("Server: %s", buffer);

                // HELO
                strcpy(buffer, "HELO");
                send(sock, buffer, strlen(buffer), 0);
                recv(sock, buffer, sizeof(buffer), 0);
                printf("Server: %s", buffer);

                // Menu:
                printf("Recipient email: ");
                scanf("%s", mail);
                while (getchar() != '\n');
                printf("Message: ");
                fgets(body, sizeof(body), stdin);
                body[strcspn(body, "\n")] = 0;

                // RCPT TO
                sprintf(buffer, "RCPT<SP>TO<CRLF>%s<CRLF>", mail);
                send(sock, buffer, strlen(buffer), 0);
                recv(sock, buffer, sizeof(buffer), 0);
                printf("Server: %s", buffer);

                // DATA
                sprintf(buffer, "DATA<CRLF>%s", body);
                send(sock, buffer, strlen(buffer), 0);
                recv(sock, buffer, sizeof(buffer), 0);
                printf("Server: %s", buffer);
                

                
                

            }
            
            
            
            
        }
    }

}
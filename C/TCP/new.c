#include<stdio.h>
#include<arpa/inet.h>
#include<sys/types.h>

int main(){
    int sock;
    struct sockaddr_in saddr;

    sock=socket(AF_INET,SOCK_STREAM,0);
    if (sock<0){
        printf("socket creation failed");



        
    }
    else{
        saddr.sin_port=htons(8000);
        saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
        saddr.sin_family=AF_INET;

        if((bind(sock,(struct sockaddr*)&saddr,1))<1){
            printf("binding failed");

        }
        else{
            if (listen(sock,2)<0){
                printf("listen failed");
            }
            else
        }
    }
}
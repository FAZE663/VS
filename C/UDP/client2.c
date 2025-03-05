#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>


int main(){
    int sockfd;
    struct sockaddr_in saddr;
    int clen=sizeof(saddr);

    char message[50];


    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd < 0){
        printf("error in creating socket\n");
    }
    else{
        printf("socket created\n");

        saddr.sin_family=AF_INET;
        saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
        saddr.sin_port=htons(8000);

        int s1=sendto(sockfd,NULL,0,0,(struct sockaddr*)&saddr,clen);
        if (s1<0){
            printf("failed sending message\n");
        }
        else{
            printf("details sent succesfullty\n");
            int r1 = recvfrom(sockfd,message,sizeof(message),0,(struct sockaddr*)&saddr,&clen);

            if (r1<0){
                printf("message not recieved");

            }
            else{
                printf("message recieved succesfully:\n\t%s\n",message);
            }
        }
    }

}
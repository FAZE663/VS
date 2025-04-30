#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>



int main(){
    printf("Welcome to server programming\n");

    int sockfd,isock;
    struct sockaddr_in saddr,caddr;
    sockfd = socket(AF_INET,SOCK_STREAM,0);

    if(sockfd>0){
        printf("socket created: %d\n",sockfd);

        saddr.sin_family=AF_INET;
        saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        saddr.sin_port = htons(8000);

        if (bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr))>=0){
            printf("binding good\n");
            
            if (listen(sockfd,5)==0){
                printf("now acceping connnections\n");

                int clen = sizeof(caddr);
                isock = accept(sockfd,(struct sockaddr*)&caddr,&clen);
                if (isock > 0){
                    printf("connection Established: %d\n",isock);

                    //recieve packets and send ACK
                    int packetno;
                    int fail=-1;
                    while(1){

                        recv(isock,&packetno,sizeof(int),0);
                        int randval=rand();
                        if (randval%10 <3){ //30 percenthcance of not acknowledging
                            printf("simulating packet drop\n");
                            int s1 = send(isock,&fail,sizeof(int),0);
                            
                        }
                        else{
                            int s1 = send(isock,&packetno,sizeof(int),0);
                            if (s1>0){
                                printf("sent ack for packet no: %d\n", packetno);
                            }
                        }
                    }


                }
                else{
                    printf("error");
                    return 0;
                }
            }

        }
    }
}

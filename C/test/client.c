#include<stdio.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>

int main(){
    printf("the client process\n");

    int sockfd,isock;
    struct sockaddr_in saddr;

    sockfd= socket(AF_INET,SOCK_STREAM,0);

    if (sockfd > 0){
        printf("socket created\n");

        saddr.sin_family=AF_INET;
        saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        saddr.sin_port = htons(8000);

        isock=connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
        if (isock<0){
            printf("connection failed\n");

        }
        else{
            printf("connection succesfull\n");
            int j =0;
            int packetno;
            int windowsize=5;

            int start=0;

            while(j<=15){
                for ( j=start;j<start+windowsize && j<15;j++){
                    if(send(sockfd , &j , sizeof(int),0)>0){
                        printf("sent packet no : %d\n",j);
                    }
                }
                for ( j=start;start+windowsize;j++){
                    if (recv(sockfd,&packetno,sizeof(int),0)>0){
                        if (packetno==j){
                            
                            printf("ack recieved for packet %d\n",j);
                            if (j==start+windowsize){
                                start=j+1;
                            }
                            else{
                                j++;
                            }
                            
                            
                        }
                        else{
                            printf("ACK not recieved. resending all packets from the window\n");
                            break;
                        }
                    }
                }

                
                
                

            }
            
        }

    }
}
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int main(){
	int sock,client1,i,j,temp,client2;
	
	int arr[5];
	char message[20];
	struct sockaddr_in saddr,caddr1,caddr2;
	if((sock=socket(AF_INET,SOCK_STREAM,0))<0){
		printf("socket error\n");
	}
	else{
		printf("socket created\n");
		saddr.sin_family=AF_INET;
		saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
		saddr.sin_port=htons(8000);
		
		
		if(bind(sock,(struct sockaddr*)&saddr,sizeof(saddr))<0){
			printf("binding error\n");
		}
		else{
			if(listen(sock,5)<0){
				printf("error listening\n");
			}
			else{
				int clen=sizeof(caddr1);
				client1=accept(sock,(struct sockaddr*)&caddr1,&clen);
				if (client1<0){
					printf("connectioin to client1 failed\n");
					
				}
				else{

					printf("connected to client1\n");
					client2=accept(sock,(struct sockaddr*)&caddr2,&clen);
					if(client2 <0){
						printf("connection to client2 failed\n");
					}
					else{
						printf("connected to client2\n");


						
					}
					

							

				}
				
			}
		}
	}
}

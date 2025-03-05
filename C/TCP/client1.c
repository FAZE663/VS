#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int main(){
	int sock,isock;
	struct sockaddr_in saddr;
	int arr[5]={9,8,5,4,3};
	float val[3];
	
	if((sock=socket(AF_INET,SOCK_STREAM,0))<0){
		printf("socket error\n");
	}
	else{
		printf("socket created\n");
		saddr.sin_family=AF_INET;
		saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
		saddr.sin_port=htons(8000);
		
		isock=connect(sock,(struct sockaddr*)&saddr,sizeof(saddr));
		
		if (isock<0){
			printf("failed to connect\n");
			
		}
		else{
			printf("connection established\n");
		}
	}
}

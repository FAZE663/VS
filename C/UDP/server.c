#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>


char reversestr(char *string){
    int left=0,right=strlen(string)-1;
    while (left<right){
        char temp=string[left];
        string[left]=string[right];
        string[right]=temp;
        left++;
        right--;
    }
    
}


int main(){
    int sockfd,client1,client2;
    struct sockaddr_in saddr,caddr1,caddr2;
    int clen=sizeof(caddr1);

    //data type
    char message[50];

    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd<0){
        printf("error creating socket\n");
    }
    else{
        printf("socket created succesfully\n");

        saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
        saddr.sin_family=AF_INET;
        saddr.sin_port=htons(8000);

        if(bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr))<0){
            printf("binding failed\n");
        }
        else{
            printf("binding completed\n");

            int r1=recvfrom(sockfd,message,sizeof(message),0,(struct sockaddr*)&caddr1,&clen);
            int r2=recvfrom(sockfd,NULL,0,0,(struct sockaddr*)&caddr2,&clen);

            if (r1>1){
                printf("message recieved:\n\t%s\n",message);
            }
            reversestr(message);
            printf("reversed string succesfully\n\t%s\n",message);

            int s1=sendto(sockfd,message,sizeof(message),0,(struct sockaddr*)&caddr2,clen);




            close(sockfd);
        }

    }
}

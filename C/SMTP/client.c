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
    char buffer[1024];

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
                int choice;
                printf("1.check mail\n2.send mail\n");
                scanf("%d",&choice);
                if (choice==1){
                    int rmail=recv(isock,buffer,sizeof(buffer),0);
                    if(rmail>0){
                        printf("%s",buffer);
                    }
                }
                else if (choice==2)
                {
                    printf("input the recipient mail: ");
                    char mail[50],body[100];
                    scanf("%s",mail);
                    while (getchar() != '\n');  
                    printf("input message: ");
                    fgets(body, sizeof(body), stdin);
                    body[strcspn(body, "\n")] = 0; 

                    sprintf(buffer,"RCPT<SP>TO<CRLF>%s<CRLF>DATA<CRLF>%s\0",mail,body);
                    int s2=send(sock,buffer,sizeof(buffer),0);
                    if (s2>0){
                        printf("mail sent");
                    } 
                }
                

            }
            
            
            
            
        }
    }

}
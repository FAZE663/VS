#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<unistd.h>
#include<time.h>

int main(){
    int sockfd, isock, arq;
    struct sockaddr_in caddr, saddr;
    int clen = sizeof(caddr), tmp;

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        printf("Error Creating Socket\n");
        return 0;
    }

    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8000);

    if (bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0){
        printf("Error While Binding\n");
        return 0;
    }
    
    printf("Socket created and bound.\nWaiting for connection...\n");
    recvfrom(sockfd, NULL, 0, 0, (struct sockaddr*)&caddr, &clen);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    srand(time(NULL));

    while (1){
        printf("Select the type of ARQ:\n1. Stop N Wait\n2. Go Back N\n3. Selective Repeat\n");
        scanf("%d", &arq);
        int i = 0, window_size = 4, base = 0, next_seq = 0;
        int received[window_size * 2];

        switch (arq)
        {
        case 1:
            while (i < 11){
                if (rand() % 10 < 3){
                    printf("Packet %d LOST\n", i);
                    sleep(1);
                    continue;
                }
                sendto(sockfd, &i, sizeof(i), 0, (struct sockaddr*)&caddr, clen);
                printf("Packet %d sent\n", i);

                if (recvfrom(sockfd, &tmp, sizeof(tmp), 0, (struct sockaddr*)&caddr, &clen) > 0){
                    if (tmp == i){
                        printf("ACK %d received\n", i);
                        i++;
                    }
                } else {
                    printf("Timeout, resending packet %d...\n", i);
                }
                sleep(1);
            }
            break;

        case 2:
            while (base < 11){
                while (next_seq < base + window_size && next_seq < 11){
                    if (rand() % 10 < 3){
                        printf("Packet %d LOST\n", next_seq);
                    } else {
                        sendto(sockfd, &next_seq, sizeof(next_seq), 0, (struct sockaddr*)&caddr, clen);
                        printf("Packet %d sent\n", next_seq);
                    }
                    next_seq++;
                }

                if (recvfrom(sockfd, &tmp, sizeof(tmp), 0, (struct sockaddr*)&caddr, &clen) > 0){
                    printf("ACK %d received\n", tmp);
                    base = tmp + 1;
                } else {
                    printf("Timeout, resending from packet %d\n", base);
                    next_seq = base;
                }
                sleep(1);
            }
            break;

        case 3:
            #define WINDOW_SIZE 4
            #define TOTAL_PACKETS 11
            int received[TOTAL_PACKETS] = {0};
            int tmp, i = 0;

            while (i < TOTAL_PACKETS) {
                for (int j = 0; j < WINDOW_SIZE && i + j < TOTAL_PACKETS; j++) {
                    if (rand() % 10 < 3) {
                        printf("Packet %d LOST\n", i + j);
                    } else {
                        if (received[i + j] != 1) {
                            int val=i+j;
                            sendto(sockfd,&val, sizeof(int), 0, (struct sockaddr*)&caddr, clen);
                            printf("Packet %d sent\n", i + j);
                        }
                    }
                }

                for (int j = 0; j < WINDOW_SIZE && i + j < TOTAL_PACKETS; j++) {
                    if (recvfrom(sockfd, &tmp, sizeof(tmp), 0, (struct sockaddr*)&caddr, &clen) > 0) {
                        if (received[tmp] != 1) {
                            printf("ACK %d received\n", tmp);
                            received[tmp] = 1;
                        }
                    } else {
                        printf("Timeout for packet %d, resending...\n", i + j);
                    }
                    sleep(1);
                }

                while (received[i]) {
                    i++;
                }
            }

        default:
            printf("Invalid choice. Try again!\n");
            break;
        }
    }
}

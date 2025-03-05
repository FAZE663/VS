#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<unistd.h>

#define WINDOW_SIZE 4
#define TOTAL_PACKETS 11

int main(){
    int sockfd, tmp, arq;
    struct sockaddr_in saddr;
    int clen = sizeof(saddr);

    struct timeval timeout;
    timeout.tv_sec = 1;  
    timeout.tv_usec = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        printf("Error Creating Socket\n");
        return 0;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    saddr.sin_port = htons(8000);

    int s1 = sendto(sockfd, NULL, 0, 0, (struct sockaddr*)&saddr, clen);
    if (s1 < 0){
        printf("Failed sending connection data\n");
    } else {
        printf("Connected Successfully\n");

        while (1) {
            printf("Select the type of ARQ:\n\t1. Stop N Wait\n\t2. Go Back N\n\t3. Selective Repeat\n");
            scanf("%d", &arq);
            int i = 0, last_ack = -1;

            switch (arq) {
                case 1: // Stop-and-Wait
                    while (i < TOTAL_PACKETS) {
                        int r1 = recvfrom(sockfd, &tmp, sizeof(tmp), 0, (struct sockaddr*)&saddr, &clen);
                        if (r1 < 0) {
                            printf("Timeout! No packet received.\n");
                        } else {
                            printf("Received packet no %d\nSending ACK...\n", tmp);
                            s1 = sendto(sockfd, &tmp, sizeof(tmp), 0, (struct sockaddr*)&saddr, clen);
                            if (s1 >= 0) {
                                if (tmp==i){
                                    printf("ACK for packet no %d sent successfully\n", tmp);
                                    i++;
                                }
                                
                            }
                        }
                    }
                    break;

                case 2: // Go-Back-N
                    while (i < TOTAL_PACKETS) {
                        int r2 = recvfrom(sockfd, &tmp, sizeof(tmp), 0, (struct sockaddr*)&saddr, &clen);
                        if (r2 < 0) {
                            printf("Timeout! No packet received.\n");
                        } else {
                            printf("Received packet no %d\n", tmp);
                            if (tmp == last_ack + 1) {
                                last_ack = tmp;
                                printf("Sending cumulative ACK %d\n", last_ack);
                                sendto(sockfd, &last_ack, sizeof(last_ack), 0, (struct sockaddr*)&saddr, clen);
                                i++;
                            } else {
                                printf("Out-of-order packet. Ignoring.\n");
                            }
                        }
                    }
                    break;

                case 3: // Selective Repeat
                    #define TOTAL_PACKETS 11
                    int buffer[TOTAL_PACKETS] = {0};
                    int received[TOTAL_PACKETS] = {0};
                    int i = 0;
                    while (i < TOTAL_PACKETS) {
                        int r3 = recvfrom(sockfd, &tmp, sizeof(tmp), 0, (struct sockaddr*)&saddr, &clen);
                        if (r3 < 0) {
                            printf("Timeout! No packet received.\n");
                        } else {
                            printf("Received packet no %d\nSending selective ACK...\n", tmp);
                            if (received[tmp] != 1) {
                                received[tmp] = 1;
                                sendto(sockfd, &tmp, sizeof(tmp), 0, (struct sockaddr*)&saddr, clen);
                            }
                        }

                        while (received[i]) {
                            if (i>=TOTAL_PACKETS){
                                break;
                            }
                            printf("Delivered packet no %d\n", i);
                            received[i] = 0;
                            i++;
                        }
                    }
                    break;

                default:
                    printf("Invalid choice. Try again!\n");
                    break;
            }
        }
    }

    close(sockfd);
    return 0;
}

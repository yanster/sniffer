#define SRV_IP "172.93.53.91"

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <string.h>
 #include <signal.h>
 #include <sys/time.h>
 #include <time.h>
 #include <errno.h>
 #include <err.h>
 #include <sys/socket.h>
 #include <net/if.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>

 int sniffer;
 struct sockaddr_in servaddr;

 int main(void) { 

    printf("here");

    char message[100];
    
    if ((sniffer=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(3333);
    //servaddr.sin_addr.s_addr = inet_addr("172.93.53.91");
    memset(servaddr.sin_zero, '\0', sizeof servaddr.sin_zero);  

    if (inet_aton(SRV_IP, &servaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    snprintf(message, sizeof(message), "test");

    printf("%s", message);

    //send the message
    if (sendto(sniffer, message, strlen(message)+1, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("sendto() error");
    }

    return 0;

 }
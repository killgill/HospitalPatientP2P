/*
        demo-udp-03: udp-send: a simple udp client
    send udp messages
    This sends a sequence of messages (the # of messages is defined in MSGS)
    The messages are sent to a port defined in HCPORT 

        usage:  udp-send

        Paul Krzyzanowski
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "port.h"
#include <ctime>

#define BUFLEN 2048
#define MSGS 5  /* number of messages to send */

int main(void)
{
    //chooses doctor randomly
    unsigned seed = time(0);
    srand(seed);
    int doctor = 1 + (rand()%2);

    //Create TCP socket dynamically
    struct addrinfo hints, *p1info; //*p1info points to results

    memset(&hints, 0, sizeof hints); //ensures empty struct
    hints.ai_family = AF_UNSPEC; //ipv4 or v6 is fine
    hints.ai_socktype = SOCK_STREAM; //TCP stream sockeet
    hints.ai_flags= AI_PASSIVE; //fill in IP

    // checks for error with getaddrinfo and prints debug info
    // also ensures that p1info points to the addrinfos from the struct
    int status;
    if ((status = getaddrinfo(NULL,0,&hints,&p1info)) != 0) {
        fprintf(stderr, "gettaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    int sockfd = socket(PF_INET,SOCK_STREAM,0); //creates socket

    //this block sorts out the address is already in use error
    int yes = 1;
    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    }

    bind(sockfd, p1info->ai_addr, p1info->ai_addrlen); //binds socket to port from getaddrinfo()

    struct sockaddr_in myaddr, remaddr;
    int fd, i, slen=sizeof(remaddr);
    char buf[BUFLEN];   /* message buffer */
    int recvlen;        /* # bytes in acknowledgement message */
    char *server = "127.0.0.1"; /* change this to use a different server */

    //
    //
    //
    //
    //
    /* create a UDP socket */

    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");

    /* bind it to all local addresses and pick any port number */

    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(P1PORT);

    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
        return 0;
    }       

    /* now define remaddr, the address to whom we want to send messages */
    /* For convenience, the host address is expressed as a numeric IP address */
    /* that we will convert to a binary format via inet_aton */

    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(HCPORT);
    if (inet_aton(server, &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* now let's send the messages */
    sprintf(buf, "patient1 %s %d doctor%d \n", server, P1TCP, doctor);
    if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {
        perror("sendto");
        exit(1);
    }
    /* now receive an acknowledgement from the server */
    // recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, (unsigned int*)&slen);
    //         if (recvlen >= 0) {
    //                 buf[recvlen] = 0;   /* expect a printable string - terminate it */
    //                 printf("patient1: \"%s\"\n", buf);
    //         }

    close(fd);
}

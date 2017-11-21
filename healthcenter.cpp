/*
        demo-udp-03: udp-recv: a simple udp server
    receive udp messages

        usage:  udp-recv

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
#include "healthcenter.h"
#include <iostream>
#include <fstream>


#define BUFSIZE 2048
using namespace std;

int
main(int argc, char **argv)
{
    struct sockaddr_in myaddr;  /* our address */
    struct sockaddr_in remaddr; /* remote address */
    struct sockaddr_in d1addr;
    struct sockaddr_in d2addr;
    socklen_t addrlen = sizeof(remaddr);        /* length of addresses */
    int recvlen;            /* # bytes received */
    int fd;             /* our socket */
    int msgcnt = 0;         /* count # of messages we received */
    char buf[BUFSIZE]; /* receive buffer */
    char *server = "127.0.0.1"; /* change this to use a different server */


    /* create a UDP socket */

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket\n");
        return 0;
    }

    /* bind the socket to any valid IP address and a specific port */

    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(HCPORT);

    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
        return 0;
    }
    int numPatients = 0;
    string patientInfo[4];

    /* now loop, receiving data and printing what we received */
    for (;;) {
        if (numPatients == 4){
            break;
        }
        recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
        if (recvlen > 0) {
            buf[recvlen] = 0;
            printf("healthcenter: \"%s\n", buf);
            patientInfo[numPatients] = buf;
            numPatients++;         
        }
        else {
            printf("uh oh - something went wrong!\n");
        }
    }
    ofstream myfile;
    myfile.open ("directory.txt");
    for (int i = 0; i < 4; ++i)
    {
        myfile << patientInfo[i] << "\n";

    }
    myfile.close();
    printf("Registration of peers completed! Run the doctors!");

    memset((char *) &d1addr, 0, sizeof(d1addr));
    d1addr.sin_family = AF_INET;
    d1addr.sin_addr.s_addr = htonl(INADDR_ANY);
    d1addr.sin_port = htons(D1PORT);
    if (inet_aton(server, &d1addr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    memset((char *) &d2addr, 0, sizeof(d2addr));
    d2addr.sin_family = AF_INET;
    d2addr.sin_addr.s_addr = htonl(INADDR_ANY);
    d2addr.sin_port = htons(D2PORT);
    if (inet_aton(server, &d2addr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    int numDoctors = 0;

    for (;;) {
        if (numDoctors == 2){
            break;
        }
        recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&d1addr, &addrlen);
        if (recvlen > 0) {
            buf[recvlen] = 0;
            printf("healthcenter: incoming message from doctor%s\n", buf[6]);
            switch(buf[6]) {
                case 1:
                    if (buf == "doctor1 dct1") {
                        if (sendto(fd, "directory.txt", strlen(buf), 0, (struct sockaddr *)&d1addr, addrlen) < 0)
                            perror("sendto");
                    }
                    else {
                        printf("doctor1 is a phony\n");
                    }
                    break;
                case 2:
                    if (buf == "doctor2 dct2") {
                        if (sendto(fd, "directory.txt", strlen(buf), 0, (struct sockaddr *)&d2addr, addrlen) < 0)
                            perror("sen");
                    }
                    else {
                        printf("doctor2 is a phony\n");
                    }
                    break;                
                }
            numDoctors++;         
        }
        else {
            printf("uh oh - something went wrong!\n");
        }
    }
    exit(0);
}
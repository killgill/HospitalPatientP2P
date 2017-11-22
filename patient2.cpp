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
#include "healthcenter.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>

#define BUFLEN 2048
#define MSGS 5  /* number of messages to send */
using namespace std;

int main(void)
{
    sleep(2);
    //chooses doctor randomly
    unsigned seed = time(0);
    srand(seed);
    int doctor = 1 + (rand()%2);
    // int doctor = 1;
    //Create TCP socket dynamically
    struct addrinfo hints, *servinfo; //*servinfo points to results

    memset(&hints, 0, sizeof hints); //ensures empty struct
    hints.ai_family = AF_UNSPEC; //ipv4 or v6 is fine
    hints.ai_socktype = SOCK_STREAM; //TCP stream sockeet
    hints.ai_flags= AI_PASSIVE; //fill in IP

    // checks for error with getaddrinfo and prints debug info
    // also ensures that servinfo points to the addrinfos from the struct
    int status;
    if ((status = getaddrinfo(NULL,"0",&hints,&servinfo)) != 0) {
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

    bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen); //binds socket to port from getaddrinfo()

    int P2TCP;
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else
        P2TCP = ntohs(sin.sin_port);

    /* create a UDP socket */

    freeaddrinfo(servinfo);

    struct sockaddr_in myaddr, remaddr;
    int fd, i, slen=sizeof(remaddr);
    char buf[BUFLEN];   /* message buffer */
    int recvlen;        /* # bytes in acknowledgement message */
    char *server = "127.0.0.1"; /* change this to use a different server */

    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");

    // /* bind it to all local addresses and pick any port number */

    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(P2PORT);

    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
        return 0;
    }       

    // /* now define remaddr, the address to whom we want to send messages */
    // /* For convenience, the host address is expressed as a numeric IP address */
    //  that we will convert to a binary format via inet_aton 

    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(HCPORT);
    if (inet_aton(server, &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* now let's send the messages */
    sprintf(buf, "patient2 %s %d doctor%d", server, P2TCP, doctor);
    if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {
        perror("sendto");
        exit(1);
    }
    /* now receive an acknowledgement from the server */
    // recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, (unsigned int*)&slen);
    //         if (recvlen >= 0) {
    //                 buf[recvlen] = 0;   /* expect a printable string - terminate it */
    //                 printf("patient2: \"%s\"\n", buf);
    //         }

    close(fd);

    listen(sockfd, BACKLOG); //starts listening


        // creates new socket for communication with client
    struct sockaddr_storage clt_addr;
    socklen_t addr_size = sizeof(clt_addr);
    int new_socket= accept(sockfd, (struct sockaddr *)&clt_addr, &addr_size);

    //shutdown(sockfd, SHUT_RDWR);
    //close(sockfd); //since only one connection is required, the closes the listening socket after it has been established

    char buf2[BUFLEN] = ""; //creates empty string for the buffer with the buffer length

    int numbytes;

    // Receives the packet, and also does a small error check.
    if ((numbytes = recv(new_socket, buf2, BUFLEN-1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }

    printf("Patient2: %s\n", buf2); //prints bugger
    const char *whichdoc = buf2;
    char docnumber = buf2[21];
    printf("Patient2 joined Doctor%c\n", docnumber);

    const char *msg = "Ack";
    int msglen, bytes_sent;
    msglen = strlen(msg);
    bytes_sent = send(new_socket , msg, msglen, 0);

    // memset(buf2, 0, sizeof buf2);
    char buf3[BUFLEN];

    if ((numbytes = recv(new_socket, buf3, BUFLEN-1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }
    buf3[numbytes] = 0;

    printf("Patient2: %s\n", buf3); //prints bugger

    string temp;
    string patients;
    string bufstring3(buf3);
    istringstream bufstream3(bufstring3);
    int subscribers = 0;
    stringstream(buf3) >> subscribers;
    getline(bufstream3,temp);
    while(getline(bufstream3,temp))
    {
        patients+= temp + "\n";
    }

    bytes_sent = send(new_socket , msg, msglen, 0);

    char buf4[BUFLEN];
    if ((numbytes = recv(new_socket, buf4, BUFLEN-1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }
    buf4[numbytes] = 0;

    string bufstring4(buf4);
    istringstream bufstream4(bufstring4);
    string schedule;
    while(getline(bufstream4,temp)){
        schedule+=temp + "\n";
    }

    printf("Patient2: %s\n", buf4); //prints bugger

    bytes_sent = send(new_socket , msg, msglen, 0);

    if (subscribers > 0)
    {
        cout.flush();
        istringstream substream(patients);
        getline(substream,temp);
        string patientport;
        string patientnumber;
        stringstream(temp) >> patientnumber >> patientport;
        char *PTCP = patientport.c_str();
        // printf("port is %s\n", PTCP);
        // cout << "We made it";

        struct addrinfo hints2, *cl; //*cl points to results

        memset(&hints2, 0, sizeof hints2); //ensures empty struct
        hints2.ai_family = AF_UNSPEC; //doesn't matter if IPv4 or IPv6
        hints2.ai_socktype = SOCK_STREAM; //TCP stream
        hints2.ai_flags= AI_PASSIVE; //local IP

        // checks for error with getaddrinfo and prints debug info
        // also ensures that clpoints to the addrinfos from the struct
        int status2;
        if ((status2 = getaddrinfo(NULL,PTCP,&hints2,&cl)) != 0) {
            fprintf(stderr, "gettaddrinfo error: %s\n", gai_strerror(status2));
            exit(1);
        }

        // create the socket
        int cl_skt = socket(cl->ai_family, cl->ai_socktype, cl->ai_protocol);

        //sorts out address in use errors
        int yes2 = 1;
        if (setsockopt(cl_skt,SOL_SOCKET,SO_REUSEADDR,&yes2,sizeof yes2) == -1) {
            perror("setsockopt");
            exit(1);
        }   

        //connect to the server
        connect(cl_skt,cl->ai_addr,cl->ai_addrlen);

        //generate and send message

        const char *msg = whichdoc;
        int msglen;
        msglen = strlen(msg);
        send(cl_skt, msg, msglen, 0);

        char buf2[BUFLEN];
        int numbytes;
        if ((numbytes = recv(cl_skt, buf2, BUFLEN-1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        printf("Patient2: %s\n", buf2);

        string subs = to_string(subscribers-1) + " more subscriber(s)"+ "\n";
        istringstream patientss(patients);
        getline(patientss,temp);
        while(getline(patientss,temp)){
            subs+=temp + "\n";
        }
        msg = subs.c_str();
        msglen = strlen(msg);
        send(cl_skt, msg, msglen, 0);

        if ((numbytes = recv(cl_skt, buf2, BUFLEN-1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        printf("Patient2: %s\n", buf2);

        string sched;
        istringstream scheduless(schedule);
        getline(scheduless,temp);
        while(getline(scheduless,temp)){
            sched+=temp + "\n";
        }
        msg = sched.c_str();
        msglen = strlen(msg);
        send(cl_skt, msg, msglen, 0);

        if ((numbytes = recv(cl_skt, buf2, BUFLEN-1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        printf("Patient2: %s\n", buf2);
    }

    exit(0);
}
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
    sleep(5);
    //chooses doctor randomly
    unsigned seed = time(0);
    srand(seed);
    int doctor = 1 + (rand()%2);

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

    int D1TCP;
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else
        D1TCP = ntohs(sin.sin_port);

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
    myaddr.sin_port = htons(D1PORT);

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
    sprintf(buf, "doctor1 dct1");
    if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {
        perror("sendto");
        exit(1);
    }
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, (unsigned int*)&slen);
            if (recvlen > 0) {
                    buf[recvlen] = 0;   /* expect a printable string - terminate it */
                    printf("doctor1: \"%s\"\n", buf);
            }
    
    string directory(buf);
    ifstream docfile;
    docfile.open ("doctor1.txt");
    string temp;
    string schedule;
    while(getline(docfile,temp)){
        schedule+= temp;
        schedule+= "\n";
    }
    docfile.close();
    cout.flush();
    ifstream patientfile;
    patientfile.open (directory);
    string patients;
    string patientnumber;
    string patientip;
    string patienttcp;
    string patientdoctor;
    int subscribers = 0;
    while(getline(patientfile,temp)){
        stringstream(temp) >> patientnumber >> patientip >> patienttcp >> patientdoctor;
        if (patientdoctor == "doctor1") {
            patients+= patientnumber + " " + patienttcp;
            patients+= "\n"; 
            subscribers++;           
        }
    }
    cout.flush();
    switch(subscribers) {
        case 0:
            cout << "Doctor1 has no peer subscribers\n";
            exit(0);
        case 1:
            cout << "Doctor1 has only one patient subscriber\n";
            break;
        default:
            cout << "Doctor1 has " << subscribers << " patients!\n";
            break;
    }

    close(fd);

    getline(stringstream(patients),temp);
    string patientport;
    stringstream(temp) >> patientnumber >> patientport;
    char *PTCP = patientport.c_str();
    cout.flush();
    // printf("the port number is %s\n", PTCP);

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
    const char *msg = "Welcome to the Doctor1 group\n";
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
    printf("Doctor1: %s\n", buf2);

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
    printf("Doctor1: %s\n", buf2);

    string sched;
    istringstream scheduless(schedule);
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
    printf("Doctor1: %s\n", buf2);

    close(cl_skt);

    exit(0);
}
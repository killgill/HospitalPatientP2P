#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <fstream>

#define HCPORT "30285"    // the port users will be connecting to

#define MAXBUFLEN 1000

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, HCPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("Healthcenter ready to receive phase 1\n");

    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }

    printf("Healthcenter: %s\n",
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s));
    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("healthcenter: packet contains \"%s\"\n", buf);

    close(sockfd);

    return 0;
}













// 	listen(sockhc, BACKLOG); //starts listening

// 	// creates new socket for communication with client
// 	struct sockaddr_storage clt_addr;
// 	socklen_t addr_size = sizeof(clt_addr);
// 	int new_socket= accept(sockhc, (struct sockaddr *)&clt_addr, &addr_size);

// 	shutdown(sockhc, SHUT_RDWR);
// 	close(sockhc); //since only one connection is required, the closes the listening socket after it has been established

// 	char buf[MAXDATASIZE] = ""; //creates empty string for the buffer with the buffer length

// 	int numbytes;

// 	// Receives the packet, and also does a small error check.
// 	if ((numbytes = recv(new_socket, buf, MAXDATASIZE-1, 0)) == -1)
// 	{
// 		perror("recv");
// 		exit(1);
// 	}

// 	printf("%s\n", buf); //prints bugger

// 	// generates and sends message to client
// 	const char *msg = "OK!";
// 	int msglen, bytes_sent;
// 	msglen = strlen(msg);
// 	bytes_sent = send(new_socket , msg, msglen, 0);

// 	shutdown(new_socket, SHUT_RDWR);
//     close(new_socket);//closes socket to free up the port

// 	exit(0);

//}

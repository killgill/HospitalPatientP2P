#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#define MYPORT "30285" //port from 2 + last digits of student ID
#define BACKLOG 0//pending connections the queue will hold - we only establish 1 connection, so we don't want any
#define MAXDATASIZE 100 //max number of bytes we can get at once

int main()
{


	struct addrinfo hints, *servinfo; //*servinfo points to results

	memset(&hints, 0, sizeof hints); //ensures empty struct
	hints.ai_family = AF_UNSPEC; //ipv4 or v6 is fine
	hints.ai_socktype = SOCK_STREAM; //TCP stream sockeet
	hints.ai_flags= AI_PASSIVE; //fill in IP

	// checks for error with getaddrinfo and prints debug info
	// also ensures that servinfo points to the addrinfos from the struct
	int status;
	if ((status = getaddrinfo(NULL,MYPORT,&hints,&servinfo)) != 0) {
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

	listen(sockfd, BACKLOG); //starts listening

	// creates new socket for communication with client
	struct sockaddr_storage clt_addr;
	socklen_t addr_size = sizeof(clt_addr);
	int new_socket= accept(sockfd, (struct sockaddr *)&clt_addr, &addr_size);

	shutdown(sockfd, SHUT_RDWR);
	close(sockfd); //since only one connection is required, the closes the listening socket after it has been established

	char buf[MAXDATASIZE] = ""; //creates empty string for the buffer with the buffer length

	int numbytes;

	// Receives the packet, and also does a small error check.
	if ((numbytes = recv(new_socket, buf, MAXDATASIZE-1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}

	printf("%s\n", buf); //prints bugger

	// generates and sends message to client
	const char *msg = "OK!";
	int msglen, bytes_sent;
	msglen = strlen(msg);
	bytes_sent = send(new_socket , msg, msglen, 0);

	shutdown(new_socket, SHUT_RDWR);
    close(new_socket);//closes socket to free up the port

	exit(0);

}

/*
 * ezsocks - making the process of using sockets on C as easy as wearing socks
 * a mini project by toranova
 * originally - simplesocks.c
 *
 * author:toranova
 * mailto:chia_jason96@live.com
 */

#include "eztcp.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

//wrapper
int sockclose(int sock){
	int retval;
	retval = close(sock);
	if(retval < 0)perror("sockclose");
	return retval;
}

int sockwriteshutdown(int sock){
	int retval;
	retval = shutdown(sock, 1);
	if(retval < 0)perror("sockshutdown.write");
	return retval;
}

int sockreadshutdown(int sock){
	int retval;
	retval = shutdown(sock, 0);
	if(retval < 0)perror("sockshutdown.read");
	return retval;
}

int sockgen(int timeout_sec, short reuse, short nonblock){
	// create the socket connection
	// timeout_sec - connection timeout in seconds
	// if reuse is set to 1, binds immediately again
	// if nonblock is set to 1, socket is non blocking on recv/send
	int sock;

	struct timeval timeout; //setup the timeout
	timeout.tv_sec = timeout_sec;
	timeout.tv_usec = 0;

	sock = socket(
		AF_INET,
		nonblock? SOCK_STREAM | SOCK_NONBLOCK : SOCK_STREAM ,
		0); //set protocol to 0 perhaps STREAM only supported by TCP

	//perform timeout setup for SEND and RECEIVE
	if(setsockopt(sock,SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval)) < 0){
		perror("sockgen setsockopt.sndtimeout");
		return -1;
	}
	if(setsockopt(sock,SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)) < 0){
		perror("sockgen setsockopt.rcvtimeout");
		return -1;
	}
	if(setsockopt(sock,SOL_SOCKET, SO_REUSEADDR, &reuse,sizeof(int)) < 0){
		perror("sockgen setsockopt.reuse");
		return -1;
	}
	return sock;
}

int sockconn(int sock, const char *srvaddr,int portnum){
	//connect to a socket server
	int retval = -1;
	struct sockaddr_in remote={0};

	//setup connection params
	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = inet_addr(srvaddr);
	remote.sin_port = htons(portnum);

	//attempt to connect, obtaining the result code as retval.
	retval = connect(sock, (struct sockaddr *)&remote, sizeof(struct sockaddr_in));
	if(retval < 0){
		perror("sockconn connect");
	}
	return retval;
}

int sockbind(int sock, const char *laddr, int portnum, int backlog){
	//bind the created socket
	int retval = -1;
	struct sockaddr_in remote={0};

	//setup connection params
	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = inet_addr(laddr);
	remote.sin_port = htons(portnum);

	retval = bind(sock,(struct sockaddr *)&remote,sizeof(struct sockaddr_in));
	retval += listen(sock, backlog);
	if(retval < 0){
		perror("sockbind (bind+listen)");
	}
	return retval;
}

int sockbindall(int sock, int portnum, int backlog){
	return sockbind(sock, "0.0.0.0", portnum, backlog);
}

int sockaccept( int server, char **peeraddr, int *peerport ){
	int retval;
	struct sockaddr_in tmp;
	socklen_t len;
	retval = accept(server, (struct sockaddr *)&tmp, &len);
	if(retval < 0){
		perror("sockaccept");
		return -1;
	}

	if(peeraddr != NULL){
		*peeraddr = (char *)malloc(sizeof(INET_ADDRSTRLEN));
		inet_ntop( AF_INET, &(tmp.sin_addr), *peeraddr, INET_ADDRSTRLEN );
	}
	if(peerport != NULL){
		*peerport = ntohs(tmp.sin_port);
	}
	return retval;
}

int sockpeerget(int sock, char **sockaddr, int *sockport){
	int retval;
	struct sockaddr_in sa;
	socklen_t len;
	retval = getpeername(sock, (struct sockaddr *)&sa, &len);
	if(retval < 0){
		perror("sockaddrsget getsockname");
		*sockaddr = NULL;
		return -1;
	}
	//get addr
	*sockaddr = (char *)malloc(sizeof(INET_ADDRSTRLEN));
	inet_ntop( AF_INET, &(sa.sin_addr), *sockaddr, INET_ADDRSTRLEN );

	//get port
	if(sockport != NULL)
		*sockport = ntohs(sa.sin_port);
	return retval;
}

int sendbuf(int sock, unsigned char *sendbuffer, size_t buflen){
	//send via the socket connection
	uint32_t bleft = buflen;
	uint32_t thisbatch = 0;
	uint32_t ptrindex = 0;
	int sent = 0;

	//code enters here when setup is successful
	while( bleft > 0 ){
		if(buflen > EZTCP_BATCHSZ) thisbatch = EZTCP_BATCHSZ;
		else thisbatch = bleft;
		sent = send(sock, sendbuffer+ptrindex, thisbatch,0);
		if(sent == -1)break;
		ptrindex += sent;
		bleft -= sent;
	}
	return sent;
}

int recvbuf(int sock, unsigned char *recvbuffer, size_t buflen){
	//recv via the socket connection
	int retval = -1;
	retval = recv(sock, recvbuffer, buflen, 0);
	return retval;
}

int fixedrecvbuf(int sock, unsigned char *recvbuffer, size_t buflen){
	//recv via the socket connection
	//must receive buflen or else it will not stop.
	uint32_t bleft = buflen;
	uint32_t thisbatch = 0;
	uint32_t ptrindex = 0;
	int brecv = 0;

	while( bleft > 0 ){
		if(buflen > EZTCP_BATCHSZ) thisbatch = EZTCP_BATCHSZ;
		else thisbatch = bleft;
		brecv = recv(sock, recvbuffer+ptrindex, thisbatch,0); //here it will block
		if(brecv <= 0)break;
		ptrindex += brecv;
		bleft -= brecv;
	}
	if(brecv < 0){
		perror("fixedrecvbuf brecv");
		return brecv;
	}
	return ptrindex;
}

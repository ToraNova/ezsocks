/*
 * ezsocks - making the process of using sockets on C as easy as wearing socks
 * a mini project by toranova
 * originally - simplesocks.c
 *
 * author:toranova
 * mailto:chia_jason96@live.com
 */

#ifndef _EZTCP_H_
#define _EZTCP_H_

#include <stddef.h>
#include <arpa/inet.h>

#define EZTCP_BATCHSZ 1024

//instructs g++ to not perform name mangling here
//the nested declares are c functions
#ifdef __cplusplus
extern "C"{
#endif

/*
 * creates a socket connection timeout_sec to set send and receive timeout for that socket
 * @timeout_sec - seconds for send/recv to timeout (set to 0 for no timeout)
 * @reuse - allow quick rebinding of socket
 * @nonblock - won't wait
 *
 * returns the socket fd (socket object)
*/
int sockgen( int timeout_sec, short reuse_bool, short nonblock);

/*
 * connect to a srv:port for the sock obj
 * @sock - the socket object
 * @srv - the target server address
 * @port - the target server port
 *
 * returns 0 on success, -1 on error
 */
int sockconn( int sock, const char *srv, int port);

/*
 * bind a sock obj to an address:port and starts listening
 * @sock - the socket object
 * @listen - the listening address
 * @port - the bound port
 * @backlog - maximum queue length
 *
 * returns 0 on success, -1 on error
 */
int sockbind( int sock, const char *listen, int port, int backlog);
//like sockbind, except bound on all addresses of the server
int sockbindall( int sock, int port, int backlog);

/*
 * accepts a incoming socket connection from a bound(listening) server socket
 * @server - the server socket object
 * @sockaddr - address of the returned sockaddr struct from accept(2) call (nullable)
 * @sockport - port of the returned sockaddr struct from accept(2) call (nullable)
 *
 * returns a the client socket obj
 */
int sockaccept( int server, char **sockaddr, int *sockport);

/* get incoming socket address from sockaddr_in (its peers)
 * @sock - socket object
 * @peeraddr - peer socket bound ip [OUTPUT]
 * @peerport - peer socket bound port [OUTPUT] (can be NULL)
 *
 * returns length of sockaddr buffer
 */
int sockpeerget( int sock, char **peeraddr , int *peerport);

/*
 * sends an unsigned char buffer
 * this is sent in batches of size EZTCP_BATCHSZ (1024)
 * @sock - the socket object
 * @sendbuf - the buffer to be sent
 * @buflen - the length to be sent
 *
 * returns the number of bytes sent
 */
int sendbuf(int sock, unsigned char *sendbuf, size_t buflen);

/*
 * receives an unsigned char buffer
 * this receives buflen from recvbuf, may terminate or timeout before
 * @sock - the socket object
 * @recvbuf - the buffer to store received data
 * @buflen - the length to be received
 *
 * returns the number of bytes received
 */
int recvbuf(int sock, unsigned char *recvbuf, size_t buflen);

/*
 * receives an unsigned char buffer at a fixed length
 * this receives buflen from recvbuf, blocks if there are chars left
 * @sock - the socket object
 * @recvbuf - the buffer to store received data
 * @buflen - the length to be received
 *
 * returns the number of bytes received, -1 if unable to fulfil request
 */
int fixedrecvbuf(int sock, unsigned char *recvbuf, size_t buflen);

//this is just a wrapper around unix close from unistd.h
int sockclose(int sock);

//shutdown read/write capability of the socket (gracefully closes)
int sockwriteshutdown(int sock);
int sockreadshutdown(int sock);

#ifdef __cplusplus
};
#endif

#endif

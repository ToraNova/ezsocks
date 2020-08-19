/*
 * ezsock test program (tcp client)
 */

#include "../eztcp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	int rc, sock;
	char m[] = "hola amigos!@";
	unsigned char buf[1024];

	sock = sockgen( 0, 1, 0);
	rc = sockconn( sock, "127.0.0.1", 1234 );
	if(rc < 0){
		return 1;
	}

	rc = sendbuf( sock, (unsigned char *)m, strlen(m));
	printf("sent : %s\n",m);
	printf("sent : %d byte(s)\n",rc);

	rc = recvbuf( sock, buf, 1024);
	buf[rc] = '\0'; //null terminator
	printf("recv : %s\n",buf);
	printf("recv : %d byte(s)\n",rc);

	sockclose(sock);
}

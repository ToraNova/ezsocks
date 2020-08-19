/*
 * ezsock test program (tcp server)
 */

#include "../eztcp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	int rc;
	int s, c;
	unsigned char buf[1024];
	char *cip; int cp;

	s = sockgen( 0, 1, 0);
	rc = sockbindall( s, 1234, 0 );
	//rc = sockbind( s, "127.0.0.1", 1234, 0 );
	if(rc < 0){
		return 1;
	}

	//accept and obtain the connecting client's address
	//c = sockaccept(s, &cip, &cp);
	c = sockaccept(s, NULL, NULL);
	if(c < 0){
		return 1;
	}

	//obtain connecting client (peer) address
	rc = sockpeerget( c, &cip, &cp );
	printf("conn estb : %s:%d\n", cip, cp);
	free(cip);

	rc = recvbuf( c, buf, 1024);
	printf("recv      : %d byte(s)\n",rc);
	printf("echo back : %s\n",buf);

	rc = sendbuf( c, buf, rc);
	printf("sent      : %d byte(s)\n",rc);

	sockclose(c);
	sockclose(s);
}

/*
 * tls test file
 */

#include "../eztcp.h"
#include "../eztls.h"
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
	printf("tcp conn estb : %s:%d\n", cip, cp);
	free(cip);

	//init tls
	tlsinit();
	SSL *ssl;
	rc = tlsgen(c, 1, NULL, "tests/ssl/bobcert.pem", "tests/ssl/bob.key", &ssl);
	if(rc < 0){
		return 1;
	}

	rc = tlsaccept(ssl); //await tls handshake
	if(rc < 0){
		return 1;
	}

	rc = tlsread(ssl, buf, 1024);
	printf("tls recv      : %d byte(s)\n",rc);
	printf("tls echo back : %s\n",buf);

	rc = tlswrite(ssl, buf, rc);
	printf("tls sent      : %d byte(s)\n",rc);

	tlsclose(ssl);
	sockclose(c);
	sockclose(s);

	//cleanup
	tlscleanup();

	return 0;
}

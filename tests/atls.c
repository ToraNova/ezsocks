/*
 * tls test file
 */

#include "../eztcp.h"
#include "../eztls.h"
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

	//init tls
	tlsinit();

	SSL *ssl;

	//rc = tlsgen(sock, 0, NULL, NULL, NULL, &ssl); //no cacert. tlsconnect(*,0)
	//rc = tlsgen(sock, 0, "tests/ssl/cacert.pem", NULL, NULL, &ssl); //tlsconnect(*,1)
	rc = tlsgen(sock, 0, "tests/ssl/bobcert.pem", NULL, NULL, &ssl); //tlsconnect(*,1)
	if(rc < 0){
		return 1;
	}

	rc = tlsconnect(ssl,1); //initiate tls handshake
	if(rc < 0){
		return 1;
	}

	rc = tlswrite(ssl, (unsigned char *)m, strlen(m));
	printf("tls sent : %s\n",m);
	printf("tls sent : %d byte(s)\n",rc);

	rc = tlsread(ssl, buf, 1024);
	buf[rc] = '\0'; //null terminator
	printf("tls recv : %s\n",buf);
	printf("tls recv : %d byte(s)\n",rc);

	//cleanup
	tlsclose(ssl);
	tlscleanup();

	//tcp cleanup
	sockclose(sock);

	return 0;
}

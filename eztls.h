/*
 * ezsocks - making the process of using sockets on C as easy as wearing socks
 * a mini project by toranova
 * originally - simplesocks.c
 *
 * author:toranova
 * mailto:chia_jason96@live.com
 */

#ifndef _EZTLS_H_
#define _EZTLS_H_

#include <stddef.h>
#include <openssl/ssl.h>

#define EZTLS_BATCHSZ 1024

//instructs g++ to not perform name mangling here
//the nested declares are c functions
#ifdef __cplusplus
extern "C"{
#endif

//internals
SSL_CTX *_sslctxgen(short srvbool);
int _sslctxconfig(
		SSL_CTX *ctx,
		const char *cafname,
		const char *certfname,
		const char *keyfname
		);

//initialize openssl function
void tlsinit();
void tlscleanup();

int tlsgen(
		int sock, short srvbool,
		const char *cafilename,
		const char *certfilename,
		const char *keyfilename,
		SSL **ssl
);
int tlsaccept(SSL *ssl);
int tlsconnect(SSL *ssl, short verifybool);
int tlsclose(SSL *ssl);

int tlswrite(SSL *ssl, unsigned char *buf, size_t len);
int tlsread(SSL *ssl, unsigned char *buf, size_t len);


#ifdef __cplusplus
};
#endif

#endif

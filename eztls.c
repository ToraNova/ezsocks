/*
 * ezsocks - making the process of using sockets on C as easy as wearing socks
 * a mini project by toranova
 * originally - simplesocks.c
 *
 * author:toranova
 * mailto:chia_jason96@live.com
 */

#include "eztls.h"

#include <stdio.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

void tlsinit() {
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
}

void tlscleanup() {
	EVP_cleanup();
}

int tlsclose(SSL *ssl){
	SSL_shutdown(ssl);
        SSL_free(ssl);
	return 0;
}

SSL_CTX *_sslctxgen(short srvbool) {
	const SSL_METHOD *method;
	SSL_CTX *ctx;

	if(srvbool)
		//method = SSLv23_server_method();
		//method = SSLv23_method();
		method = TLS_server_method();
	else
		//method = SSLv23_client_method();
		method = TLS_client_method();
	ctx = SSL_CTX_new(method);
	if (!ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
	}
	return ctx;
}

int _sslctxconfig(
		SSL_CTX *ctx,
		const char *cafname,
		const char *certfname,
		const char *keyfname
		)
{
	SSL_CTX_set_ecdh_auto(ctx, 1); //set ecdh

	/* Set the CACERT */
	if (cafname != NULL &&
	SSL_CTX_load_verify_locations(ctx, cafname, NULL) <= 0) {
		ERR_print_errors_fp(stderr);
		return 1;
	}

	/* Set the cert */
	if (certfname != NULL &&
	SSL_CTX_use_certificate_file(ctx, certfname, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		return 1;
	}

	/* Set the key */
	if (keyfname != NULL &&
	SSL_CTX_use_PrivateKey_file(ctx, keyfname, SSL_FILETYPE_PEM) <= 0 ) {
		ERR_print_errors_fp(stderr);
		return 1;
	}

	return 0;
}

int tlsgen(int sock, short srvbool, const char *cafname, const char *certfname, const char *keyfname, SSL **ssl){
	SSL_CTX *ctx = _sslctxgen(srvbool); //generate the ssl context
	_sslctxconfig(ctx, cafname, certfname, keyfname); //config the cert, and key files

	*ssl = SSL_new(ctx);
	if( SSL_set_fd(*ssl, sock) <= 0){
		ERR_print_errors_fp(stderr);
		return -1;
	}
	SSL_CTX_free(ctx); //free
	//our mindset follows 0 as success -1 as error
	return 0;
}

int tlsaccept(SSL *ssl){
	//wait for client hello
	if (SSL_accept(ssl) <= 0) {
		ERR_print_errors_fp(stderr);
		return -1;
	}
	return 0;
}

int tlsconnect(SSL *ssl){
	//send client hello
	if (SSL_connect(ssl) <= 0) {
		ERR_print_errors_fp(stderr);
		return -1;
	}
	return 0;
}

int tlswrite(SSL *ssl, unsigned char *buf, size_t len){
	//send via the socket connection
	uint32_t bleft = len;
	uint32_t thisbatch = 0;
	uint32_t ptrindex = 0;
	int sent = 0;

	//code enters here when setup is successful
	while( bleft > 0 ){
		if(bleft > EZTLS_BATCHSZ) thisbatch = EZTLS_BATCHSZ;
		else thisbatch = bleft;
		if(thisbatch <= 0) break;
		sent = SSL_write(ssl, buf+ptrindex, thisbatch);
		if(sent <= 0){
			ERR_print_errors_fp(stderr);
			break;
		}
		ptrindex += sent;
		bleft -= sent;
	}
	return sent;
}

int tlsread(SSL *ssl, unsigned char *buf, size_t len){
	int retval;
	retval = SSL_read(ssl, buf, len);
	if(retval <= 0){
		ERR_print_errors_fp(stderr);
	}
	return retval;
}

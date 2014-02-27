#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sodium.h>
#include "base64.h"
#include <assert.h>

/*
 * Jan-Piet Mens, February 2014
 * based on work by Moritz Warning https://github.com/mwarning/libsodium-example
 */

#define BLEN	(220 * 1024)

typedef unsigned char UCHAR;

int is_zero( const UCHAR *data, int len )
{
	int i, rc = 0;

	for(i = 0; i < len; ++i) {
		rc |= data[i];
	}

	return rc;
}


int decrypt(UCHAR plain[], const UCHAR pk[], const UCHAR sk[], const UCHAR nonce[], const UCHAR encrypted[], int length)
{
	UCHAR temp_encrypted[BLEN + crypto_box_BOXZEROBYTES];
	UCHAR temp_plain[BLEN + crypto_box_BOXZEROBYTES];
	int rc;

	if (length + crypto_box_BOXZEROBYTES >= BLEN) {
		puts("fail greater max");
		return -2;
	}

	/*
	 * http://mob5.host.cs.st-andrews.ac.uk/html/d7/d46/a00002.html#gad0201c5b5acc5e1ad6bf23fb6cb82a1d
	 *
	 * Precondition:
	 *    first crypto_box_BOXZEROBYTES of ctxt be all 0.
	 *    the nonce must have size crypto_box_NONCEBYTES.
	 */

	memset(temp_encrypted, 0, crypto_box_BOXZEROBYTES);
	memcpy(temp_encrypted + crypto_box_BOXZEROBYTES, encrypted, length);

	rc = crypto_box_open(temp_plain, temp_encrypted, crypto_box_BOXZEROBYTES + length, nonce, pk, sk);

	if( rc != 0 ) {
		puts("crypto_box_open fails");
		return -1;
	}

	/*
	 * Postcondition:
	 *   first clen bytes of msg will contain the plaintext.
	 *   first crypto_box_ZEROBYTES of msg will be all 0.
	 */

	if( is_zero(temp_plain, crypto_box_ZEROBYTES) != 0 ) {
		puts("is_zero fails");
		return -3;
	}

	memcpy(plain, temp_plain + crypto_box_ZEROBYTES, crypto_box_BOXZEROBYTES + length);

	return crypto_box_BOXZEROBYTES + length - crypto_box_ZEROBYTES;
}

/*
 * Load the base64 at `path` and decode into `target`
 */

int loadkey(char *path, UCHAR *target, unsigned long tlen)
{
    int fd, n;
    char buf[BLEN], decoded[BLEN];

    memset(target, 0, tlen);
    if ((fd = open(path, O_RDONLY)) == -1) {
        perror(path);
        return (-1);
    }

    n = read(fd, buf, BLEN);
    close(fd);

    if (n >= 0) {
    	buf[n] = 0;
	n = base64_decode(buf,  decoded);

	assert(n <= tlen);
	assert(n == 32);
	memcpy(target, decoded, n);
    }
    return (n);

}

void dumpkeys(UCHAR *sk, UCHAR *pk)
{
	char *t;
	int n;

	n = base64_encode(sk, crypto_box_SECRETKEYBYTES, &t);
	assert(n == 44);
	printf("secret: %s\n", t);
	free(t);

	n = base64_encode(pk, crypto_box_PUBLICKEYBYTES, &t);
	assert(n == 44);
	printf("public: %s\n", t);
	free(t);
}

int main(int argc, char **argv)
{
	int n, payloadlen, offset, plen, c;
	UCHAR encrypted[BLEN], plaintext[BLEN];
	UCHAR sender_pubkey[crypto_box_PUBLICKEYBYTES];
	UCHAR receiver_secretkey[crypto_box_SECRETKEYBYTES];
	UCHAR nonce[crypto_box_NONCEBYTES];
	char message[BLEN], *m;
	char *p, *b_text, *b_nonce;
	char *delimiters = ":";

	if (argc != 3) {
		fprintf(stderr, "Usage: %s FROMpubkey-file TOprivkey-file\n", *argv);
		return (1);
	}

	sodium_init();

	if (loadkey(argv[1], sender_pubkey, crypto_box_PUBLICKEYBYTES) != crypto_box_PUBLICKEYBYTES) {
		fprintf(stderr, "Can't load %s: size mismatch\n", argv[1]);
		exit(2);
	}

	if (loadkey(argv[2], receiver_secretkey, crypto_box_SECRETKEYBYTES) != crypto_box_SECRETKEYBYTES) {
		fprintf(stderr, "Can't load %s: size mismatch\n", argv[2]);
		exit(2);
	}

	// dumpkeys(receiver_secretkey, sender_pubkey);

	/* Read stdin for message consisting of b64nonce : b64crypted */

	for (c = fgetc(stdin), p = message, n = 0;
		(n < BLEN) && (c != EOF);
		c = fgetc(stdin), n++) {

		*p++ = c;
		*p = 0;
	}

	/* Warning: message string is being altered by strsep() */
	m = message;
	b_nonce = strsep(&m, delimiters);
	b_text  = strsep(&m, delimiters);

	n = base64_decode(b_nonce, nonce);
	assert(n == crypto_box_NONCEBYTES);

#if 0
	/*
	 * PyNacl's encrypted output contains the nonce in the first
	 * 24 octets of the encrypted result. I'm pointing `nonce'
	 * to the beginning of that buffer.
	 */

	offset = crypto_box_NONCEBYTES;
#endif
	offset = 0;

	n = base64_decode(b_text, encrypted);
	if (n < 0) {
		fprintf(stderr, "b64decode failed to decode payload\n");
		exit(1);
	}

	payloadlen = n - offset;
	plen = decrypt(plaintext, sender_pubkey, receiver_secretkey, nonce, encrypted + offset, payloadlen);
	if (plen < 0) {
		fprintf(stderr, "Cannot decrypt payload\n");
		exit(1);
	}

	write(1, plaintext, plen);

	return 0;
}

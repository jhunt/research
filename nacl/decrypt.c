#include "common.h"

/* NOTE: this example is expected to fail, because I screwed up
         the padding.  On purpose.  Read the blog post for more:

           http://jameshunt.us/writings/nacl-padding.html
 */
int main(int argc, char **argv)
{
	int rc;
	uint8_t client_pub[32], client_sec[32];
	uint8_t server_pub[32], server_sec[32];
	uint8_t cipher[512], plain[512];
	uint8_t nonce[24];

	/* initialize memory */
	memset(cipher, 0, 512);
	memset(plain, 0, 512);

	/* "generate" nonce */
	memset(nonce, 0, 24);

	/* "generate" keys */
	memcpy(client_pub, CLIENT_PUB, 32);
	memcpy(client_sec, CLIENT_SEC, 32);
	memcpy(server_pub, SERVER_PUB, 32);
	memcpy(server_sec, SERVER_SEC, 32);

	/* assemble plaintext */
	memcpy(plain, MESSAGE, MESSAGE_LEN);
	dump("plaintext, before encryption", plain, MESSAGE_LEN);

	/* encipher message from client to server */
	rc = crypto_box(cipher, plain, MESSAGE_LEN,
	                nonce, server_pub, client_sec);
	dump("ciphertext", cipher, MESSAGE_LEN);
	assert(rc == 0);

	/* erase all trace of plaintext */
	memset(plain, 0, 512);

	/* decipher message as server, using client's public key */
	rc = crypto_box_open(plain, cipher, MESSAGE_LEN,
	                     nonce, client_pub, server_sec);
	dump("plaintext, after decryption", plain, MESSAGE_LEN);
	assert(rc == 0);

	return 0;
}

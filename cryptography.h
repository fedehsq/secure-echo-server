#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "shared.h"

struct envelope {
    int eskl;
    unsigned char *esk;
    unsigned char *ct;
};

unsigned char *generate_random_key(int key_len);

void std_symmetric_encryption(unsigned char *plaintext, unsigned char *ciphertext,
    unsigned char *key);

void std_symmetric_decryption(unsigned char *ciphertext, unsigned char *plaintext, 
    unsigned char *key);

struct envelope *envelope_encryption(unsigned char *pt, int fd);

unsigned char *envelope_decryption(struct envelope *envelope);
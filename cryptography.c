#include "cryptography.h"

int BUFFER_SIZE;
int BLOCK_SIZE;

/* key is calculate at every connection with a client, 
send it and then start the message with encrypted mess */
unsigned char *generate_random_key(int key_len) {
  unsigned char *key;
  // initilise seed
  handle_negative_ssl(RAND_poll());
  // allocate key
  handle_null(key = (unsigned char *)calloc(BLOCK_SIZE + BUFFER_SIZE, sizeof(unsigned char)),
    "calloc");
  // prg key
  handle_negative_ssl(RAND_bytes(key, key_len));
  return key;
} 

void std_symmetric_encryption(unsigned char *plaintext, unsigned char *ciphertext, 
unsigned char *key) {
  int written;
  EVP_CIPHER_CTX *context;
  handle_null_ssl(context = EVP_CIPHER_CTX_new());
  handle_negative_ssl(EVP_EncryptInit(context, EVP_aes_256_ecb(), key, NULL));
  handle_negative_ssl(EVP_EncryptUpdate(context, ciphertext, &written, plaintext,
    BUFFER_SIZE));
  handle_negative_ssl(EVP_EncryptFinal(context, &ciphertext[written], &written));
  EVP_CIPHER_CTX_free(context);
}

void std_symmetric_decryption(unsigned char *ciphertext, unsigned char *plaintext, 
unsigned char *key) {
  int written;
  EVP_CIPHER_CTX *context;
  handle_null_ssl(context = EVP_CIPHER_CTX_new());
  handle_negative_ssl(EVP_DecryptInit(context, EVP_aes_256_ecb(), key, NULL));
  // I can't know length of ct because it is in byte, but I assume the MAX_LEN
  // of a message!
  handle_negative_ssl(EVP_DecryptUpdate(context, plaintext, &written, 
    ciphertext, BLOCK_SIZE + BUFFER_SIZE));
  handle_negative_ssl(EVP_DecryptFinal(context, &plaintext[written], &written));
  EVP_CIPHER_CTX_free(context);
}

// return a triple: ek, ekl, ct => struct
struct envelope *envelope_encryption(unsigned char *pt, int fd) {
  struct envelope *env = malloc(sizeof(struct envelope));
  int written; // bytes written from the SealUpdate, SealFinal
  FILE *fp_pub_key; // path of pub key
  EVP_PKEY *public_key; // container of public key
  EVP_CIPHER_CTX *context;
  handle_null(fp_pub_key = fopen("./public_key.pem", "r"), "fopen");
  handle_null_ssl(public_key = PEM_read_PUBKEY(fp_pub_key, NULL, NULL, NULL));
  handle_null(env -> ct = (unsigned char*)calloc(BLOCK_SIZE + BUFFER_SIZE + 1, 
      sizeof(unsigned char)), "calloc");
  handle_null(env -> esk = (unsigned char*)calloc(EVP_PKEY_size(public_key) + 1, 
      sizeof(unsigned char)), "calloc");
  handle_null_ssl(context = EVP_CIPHER_CTX_new());
  handle_negative_ssl(EVP_SealInit(context, EVP_aes_256_ecb(), &env -> esk,
      &env -> eskl, NULL, &public_key, 1)); 
  handle_negative_ssl(EVP_SealUpdate(context, env -> ct, &written, pt, BUFFER_SIZE));
  handle_negative_ssl(EVP_SealFinal(context, env -> ct + written, &written));
  EVP_PKEY_free(public_key);
  EVP_CIPHER_CTX_free(context);
  handle_error(fclose(fp_pub_key), "fclose");
  return env;
}

unsigned char *envelope_decryption(struct envelope *env) {
  unsigned char *pt;
  int written; // bytes written from the SealUpdate, SealFinal
  FILE *fp_prv_key; // path of prv key
  EVP_PKEY *prv_key; // container of prv key
  EVP_CIPHER_CTX *context;
  handle_null(fp_prv_key = fopen("./private_key.pem", "r"), "fopen");
  handle_null_ssl(prv_key = PEM_read_PrivateKey(fp_prv_key, NULL, NULL, NULL));
  handle_null(pt = (unsigned char*)calloc(BLOCK_SIZE + BUFFER_SIZE + 1, 
      sizeof(unsigned char)), "calloc");
  handle_null_ssl(context = EVP_CIPHER_CTX_new());
  handle_negative_ssl(EVP_OpenInit(context, EVP_aes_256_ecb(), env -> esk,
      env -> eskl, NULL, prv_key));
  handle_negative_ssl(EVP_OpenUpdate(context, pt, &written, env -> ct, 
    BLOCK_SIZE + BUFFER_SIZE));
  handle_negative_ssl(EVP_OpenFinal(context, pt + written, &written));
  EVP_PKEY_free(prv_key);
  EVP_CIPHER_CTX_free(context);
  handle_error(fclose(fp_prv_key), "fclose");
  return pt;
}

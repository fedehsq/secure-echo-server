#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "cryptography.h"

extern int BUFFER_SIZE;
extern int BLOCK_SIZE;

void echo(int server_fd);

int main(int argc, char *argv[]) { 
  struct sockaddr_un name;
  int server_fd;
  setup_params();
  // Create local socket.
  handle_error(server_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0), "socket");
  memset(&name, 0, sizeof(struct sockaddr_un));
  name.sun_family = AF_UNIX;
  strncpy(name.sun_path, "./sock.socket", sizeof(name.sun_path) - 1);
  // Connect to socket
  handle_error(connect(server_fd, (struct sockaddr *) &name, sizeof(name)), 
    "connect");
  // start messaging
  echo(server_fd);
  return EXIT_SUCCESS;
}

void echo(int server_fd) {
  // first iteration client send to server the symmetric key using its public key
  // with RSA, then they use the simmetric key generate by client
  unsigned char *ct;
  unsigned char *pt;
  unsigned char *key = generate_random_key(256);
  struct envelope *env = envelope_encryption(key, server_fd);
   // send encrypted key len
  handle_error(write(server_fd, &env -> eskl, sizeof(env -> eskl)), "write");
  // send the encrypted key
  handle_error(write(server_fd, env -> esk, env -> eskl), "write");
  // send ct that is the symmetric key
  handle_error(write(server_fd, env -> ct, BUFFER_SIZE + BLOCK_SIZE), "write");
  // allocate the buffers
  handle_null(ct = (unsigned char *)calloc(BUFFER_SIZE + BLOCK_SIZE + 1,
      sizeof(unsigned char)), "calloc");
  handle_null(pt = (unsigned char *)calloc(BUFFER_SIZE + BLOCK_SIZE + 1,
      sizeof(unsigned char)), "calloc");
  // echo
  for (; strncmp((char *)pt, "exit", strlen("exit")) != 0; ) {
      memset(ct, 0, BUFFER_SIZE + BLOCK_SIZE);
      memset(pt, 0, BUFFER_SIZE + BLOCK_SIZE);
      handle_error(read(server_fd, ct, BUFFER_SIZE + BLOCK_SIZE), "read");
      // decrypt message and print it
      std_symmetric_decryption(ct, pt, key);
      printf("server: %s", pt);
      printf("> ");
      handle_null(fgets((char *)pt, BUFFER_SIZE, stdin), "fgets");
      // re-rencrypt pt and send to client
      std_symmetric_encryption(pt, ct, key);
      handle_error(write(server_fd, ct, BUFFER_SIZE + BLOCK_SIZE), 
      "write");
  }
  memset(ct, 0, BUFFER_SIZE + BLOCK_SIZE);
  memset(pt, 0, BUFFER_SIZE + BLOCK_SIZE);
  memset(key, 0, 256);
  memset(env -> ct, 0, BUFFER_SIZE + BLOCK_SIZE);
  memset(env -> esk, 0, env -> eskl);
  free(env -> ct);
  free(env -> esk);
  free(env);
  free(ct);
  free(key);
  free(pt);
  handle_error(close(server_fd), "close");
}
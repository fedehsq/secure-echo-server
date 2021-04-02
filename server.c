#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "cryptography.h"

extern int BUFFER_SIZE;
extern int BLOCK_SIZE;
const char *WELCOME_MESSAGE = "--- WELCOME TO A VERY SECURE ECHO SERVER --- \n";
void echo(int client_fd);

int main() {
  struct sockaddr_un name;
  int ret, connection_socket;
  setup_params();
  // Create local socket.
  handle_error(connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0), "socket");
  memset(&name, 0, sizeof(struct sockaddr_un));
  // Bind socket to socket name.
  name.sun_family = AF_UNIX;
  strncpy(name.sun_path, "./sock.socket", sizeof(name.sun_path) - 1);
  handle_error(ret = bind(connection_socket, (const struct sockaddr *) &name, 
  sizeof(struct sockaddr_un)), "bind");
  // Prepare for accepting connections. The backlog size is set
  // to 20. So while one request is being processed other requests
  // can be waiting.
  handle_error(ret = listen(connection_socket, 20), "listen");
  puts("Listening for connections");
  int client_fd;
  handle_error(client_fd = accept(connection_socket, NULL, NULL), "accept");
  // Start a thread when a client is connected
  echo (client_fd);
}

void echo(int client_fd) {
  // first iteration client send to server the symmetric key using its public key
  // with RSA, then they use the simmetric key generate by client
  struct envelope *env = malloc(sizeof(struct envelope));
  read(client_fd, &env -> eskl, sizeof(env -> eskl));
  env -> esk = (unsigned char*)calloc((env -> eskl) + 1, sizeof(unsigned char));
  read(client_fd, env -> esk, env -> eskl);
  env -> ct = (unsigned char*)calloc(BLOCK_SIZE + BUFFER_SIZE + 1, sizeof(unsigned char));
  read(client_fd, env -> ct, BLOCK_SIZE + BUFFER_SIZE);
  // read from client the symmetric key
  unsigned char *key = envelope_decryption(env);
  unsigned char *ct;
  unsigned char *pt;
  handle_null(ct = (unsigned char *)calloc(BUFFER_SIZE + BLOCK_SIZE + 1,
   sizeof(unsigned char)), "calloc");
  handle_null(pt = (unsigned char *)calloc(BUFFER_SIZE + BLOCK_SIZE + 1,
   sizeof(unsigned char)), "calloc");
  // send welcome message
  std_symmetric_encryption((unsigned char*)WELCOME_MESSAGE, ct, key);
  handle_error(write(client_fd, ct, BUFFER_SIZE + BLOCK_SIZE), "write");
  // echo
  for (;;) {
      memset(ct, 0, BUFFER_SIZE + BLOCK_SIZE);
      memset(pt, 0, BUFFER_SIZE + BLOCK_SIZE);
      handle_error(read(client_fd, ct, BUFFER_SIZE + BLOCK_SIZE), "read");
      // decrypt message and print it
      std_symmetric_decryption(ct, pt, key);
      printf("client: %s", pt);
      if (strncmp((char *)pt, "exit", strlen("exit")) == 0) {
        break;
      }
      // re-rencrypt pt and send to client
      std_symmetric_encryption(pt, ct, key);
      handle_error(write(client_fd, ct, BUFFER_SIZE + BLOCK_SIZE), 
       "write");
  }
  memset(ct, 0, BUFFER_SIZE + BLOCK_SIZE);
  memset(key, 0, BLOCK_SIZE);
  memset(pt, 0, BUFFER_SIZE + BLOCK_SIZE);
  memset(env -> ct, 0, BUFFER_SIZE + BLOCK_SIZE);
  memset(env -> esk, 0, env -> eskl);
  free(ct);
  free(key);
  free(pt);
  free(env -> esk);
  free(env -> ct);
  free(env);
  handle_error(close(client_fd), "close");
}
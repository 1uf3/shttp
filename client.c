#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  int sockd;
  struct sockaddr_un cli;
  socklen_t socklen;

  if ( argc != 2 ) {
      puts("Usage: sockconn <filename>");
      exit(EXIT_FAILURE);
    }

  if ( (sockd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Initialize
  memset(&cli, 0, sizeof(cli));
  cli.sun_family = AF_UNIX;
  strncpy(cli.sun_path, argv[1], sizeof(cli.sun_path));

  socklen = SUN_LEN(&cli);
  if (connect(sockd, (struct sockaddr *)&cli, socklen)) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  printf("connected to socket: %s\n", cli.sun_path);

  exit(EXIT_SUCCESS);
}

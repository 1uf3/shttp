#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  int sockfd;
  struct sockaddr_in cli;
  socklen_t socklen;

  if ( argc != 2 ) {
      puts("Usage: sockconn <filename>");
      exit(EXIT_FAILURE);
    }

  if ( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Initialize
  memset(&cli, 0, sizeof(cli));
  cli.sin_family = AF_INET;
  cli.sin_port = htons(50000);

  if (!(inet_aton(argv[1], &cli.sin_addr))) {
    perror("inet_aton");
    exit(EXIT_FAILURE);
  }

  socklen = sizeof(cli);
  if (connect(sockfd, (struct sockaddr *)&cli, socklen)) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  printf("connected to socket");

  exit(EXIT_SUCCESS);
}

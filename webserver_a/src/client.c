#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char** argv) {
  int sockfd;
  struct sockaddr_in cli;
  socklen_t socklen;

  if ( argc != 2 ) {
      puts("Usage: sockconn <filename>");
      exit(EXIT_FAILURE);
    }

  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Initialize
  memset(&cli, 0, sizeof(cli));
  cli.sin_family = AF_INET;
  cli.sin_port = htons(4000);
  socklen = sizeof(cli);

  if (!(inet_aton(argv[1], &cli.sin_addr))) {
    perror("inet_aton");
    exit(EXIT_FAILURE);
  }

  if (connect(sockfd, (struct sockaddr *)&cli, socklen)) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  printf("connected to socket\n");

  while(1) {
    int req, res;
    char buff[1024];
    memset(buff, 0, sizeof(buff));

    printf("req: ");
    scanf("%s", buff);

    req = write(sockfd, buff, sizeof(buff));

    if(strncmp(buff, "quit", sizeof(buff)) == 0) {
      break;
    }

    memset(buff, 0, sizeof(buff));
    res = read(sockfd, buff, sizeof(buff));
    printf("res: %s\n", buff);

    if(strncmp(buff, "quit", sizeof(buff)) == 0) {
      break;
    }
  }
  close(sockfd);
  exit(EXIT_SUCCESS);
}

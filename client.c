#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  int sockfd;
  struct sockaddr_un cli;
  socklen_t socklen;

  if ( argc != 2 ) {
      puts("Usage: sockconn <filename>");
      exit(EXIT_FAILURE);
    }

  if ( (sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Initialize
  memset(&cli, 0, sizeof(cli));
  cli.sun_family = AF_UNIX;
  strncpy(cli.sun_path, argv[1], sizeof(cli.sun_path));

  socklen = SUN_LEN(&cli);
  if (connect(sockfd, (struct sockaddr *)&cli, socklen)) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  printf("connected to socket: %s\n", cli.sun_path);

  int cnt, len;
  char buff[1024];
  while( (cnt = read(fileno(stdin), buff, sizeof(buff))) > 0 ) {
    if( len < 0 ) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    if ( (len = write(sockfd, buff, cnt)) != cnt ) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }

  exit(EXIT_SUCCESS);
}

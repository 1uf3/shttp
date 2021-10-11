#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  int sockfd, infd;
  struct sockaddr_un srv;
  socklen_t socklen;

  if ( argc != 2 ) {
      puts("Usage: mksock <filename>");
      exit(EXIT_FAILURE);
    }

  if ( (sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Initialize
  memset(&srv, 0, sizeof(srv));
  srv.sun_family = AF_UNIX;
  strncpy(srv.sun_path, argv[1], sizeof(srv.sun_path));

  if ( (bind(sockfd, (struct sockaddr *)&srv, SUN_LEN(&srv))) < 0 ) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if( listen(sockfd, 5) < 0 ) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  printf("socket available: %s\n", srv.sun_path);

  if ( (infd = accept(sockfd, (struct sockaddr *)&srv, &socklen)) >= 0 ) {
    puts("New connection granted");
  }

  int cnt, len;
  char buff[1024];
  while( (cnt = read(infd, buff, sizeof(buff))) > 0 ) {
    if (len < 0) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    if ( (len = write(fileno(stdout), buff, cnt)) != cnt ) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }

  exit(EXIT_SUCCESS);
}

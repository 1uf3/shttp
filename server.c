#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  int sockfd, infd;
  struct sockaddr_in srv;
  socklen_t socklen;
  int i = 1;

  if ( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Initialize
  memset(&srv, 0, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons(50000);

  socklen = sizeof(srv);
  if ( (bind(sockfd, (struct sockaddr *)&srv, socklen)) < 0 ) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if( listen(sockfd, 5) < 0 ) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  puts("TCP/IP socket available");
  printf("\taddr %s\n", inet_ntoa(srv.sin_addr));
  printf("\tport %d\n", ntohs(srv.sin_port));

  while ( (accept(sockfd, (struct sockaddr *)&srv, &socklen)) >= 0 ) {
    puts("New connection granted");
  }

  exit(EXIT_SUCCESS);
}

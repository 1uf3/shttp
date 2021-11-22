#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  int sock_srv, socket_cli;
  struct sockaddr_in srv;
  socklen_t srv_sock_len;
  struct sockaddr_in cli;
  socklen_t cli_sock_len;
  int i = 0;

  if ( (sock_srv = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Initialize
  memset(&srv, 0, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons(4000);
  srv.sin_addr.s_addr = INADDR_ANY;

  srv_sock_len = sizeof(srv);
  if ( (bind(sock_srv, (struct sockaddr*)&srv, srv_sock_len)) < 0 ) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if( listen(sock_srv, 1) < 0 ) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

//  char hostbuf[256];
//  char *IPbuf;
//  struct hostent* host_entry;
//
//  gethostname(hostbuf, sizeof(hostbuf));
//  host_entry = gethostbyname(hostbuf);
//  if (host_entry == NULL) {
//    perror("gethostbyname");
//    exit(EXIT_FAILURE);
//  }
//  IPbuf = inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[0]);
//  if (IPbuf == NULL) {
//    perror("inet_ntoa");
//    exit(EXIT_FAILURE);
//  }

  puts("TCP/IP socket available");
// printf("\taddr %s\n", IPbuf);
  printf("\taddr %s\n", inet_ntoa(srv.sin_addr));
  printf("\tport %d\n", ntohs(srv.sin_port));

  putchar('\n');
  puts("-----------------------------------------------------------------------------------");
  putchar('\n');
  cli_sock_len = sizeof(cli);
  if ( (socket_cli = accept(sock_srv, (struct sockaddr *)&cli, &cli_sock_len)) < 0 ) {
    close(sock_srv);
  }

  char buff[8190];
  memset(buff,0,sizeof(buff));
  int req = read(socket_cli, buff, sizeof(buff));
  if (req < 0) {
    perror("read");
    exit(EXIT_FAILURE);
  }

  // Print http request
  printf("HTTP Request: %s\n", buff);
  putchar('\n');

  // Print hex code
  while(i < 8190) {
    printf("0x%02x ", buff[i]);
    if(i % 17 == 16) {
      putchar('\n');
    }
    i++;
  }

  close(socket_cli);
  close(sock_srv);

  exit(EXIT_SUCCESS);
}

#include <stdio.h> 
#include <netdb.h> 
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_ROOT "/home/lufe/develop/network-dev/htdocs"


void getRequestFileName(const char* req, char full_path[]);
int isFileExist(char* filename);
void server_status(int socket, const char* message);

void httpd(int socket);

void printLine(void);
void error(const char* message);

int main(int argc, char *argv[]) {

  int socket_srv;
  struct sockaddr_in srv;
  socklen_t srv_sock_len;

  if ( (socket_srv = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    error("socket");
  }

  // Initialize
  memset(&srv, 0, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons(4000);
  srv.sin_addr.s_addr = INADDR_ANY;

  srv_sock_len = sizeof(srv);
  if ( (bind(socket_srv, (struct sockaddr*)&srv, srv_sock_len)) < 0 ) {
    error("bind");
  }

  if( listen(socket_srv, 10) < 0 ) {
    error("listen");
  }

  puts("TCP/IP socket available");
  // printf("\taddr %s\n", IPbuf);
  printf("\taddr %s\n", inet_ntoa(srv.sin_addr));
  printf("\tport %d\n", ntohs(srv.sin_port));

  printLine();

  while(1){
    struct sockaddr_in cli;
    socklen_t cli_sock_len;
    int socket_cli;

    cli_sock_len = sizeof(cli);
    if ( (socket_cli = accept(socket_srv, (struct sockaddr *)&cli, &cli_sock_len)) < 0 ) {
      close(socket_srv);
      error("accept");
    }

    pthread_t worker;

    if(pthread_create(&worker, NULL, (void *)httpd, &socket_cli ) != 0) {
      perror("pthread_create");
      break;
    }

  }
  close(socket_srv);
  exit(EXIT_SUCCESS);
}

void httpd(int socket){
  pthread_detach(pthread_self());

  char request[8190];
  memset(request, 0, sizeof(request));
  int request_bytes = read(socket, request, sizeof(request));
  if (request_bytes < 0) {
    error("read");
  }
  printf("HTTP Request: %s\n", request);

  printLine();

  char file_path[1024];
  memset(file_path, 0, sizeof(file_path));
  char tmp[1024];
  memset(tmp, 0, sizeof(tmp));
  getRequestFileName(request, file_path);
  printf("%s\n size%lu\n", file_path, strlen(file_path));
  printf("\n debug \n");

  char response[4000];
  memset(response, 0, sizeof(response));

  if(isFileExist(file_path) != 0) {
    strcpy(response, "HTTP/1.0 404 NotFound\r\n");
  }
  if(isFileExist(file_path) == 0) {
    strcpy(response, "HTTP/1.0 200 OK\r\n\r\n");
    FILE* fp;
    fp = fopen(file_path, "r");
    while(1) {
      char file_tmp[256];
      memset(file_tmp, 0, sizeof(file_tmp));
      if(fgets(file_tmp, 256, fp) == NULL) {
        break;
      }
      strcat(response, file_tmp);
    }
    fclose(fp);
  }
  printf("\n debug2 \n");
  fflush(stdout);
  write(socket, response, strlen(response));
  printf("\n%s\n",response);
  printf("\n debug3 \n");
  close(socket);
}

void getRequestFileName(const char* req, char* full_path) {
  char tmp[1024];
  memset(tmp, 0, sizeof(tmp));
  sscanf(req, "%[^\n]", tmp); 
  printf("request_line: %s\n", tmp);

  char *method = strtok(tmp, " ");       
  char *path = strtok(NULL, " ");  

  strncpy(full_path, SERVER_ROOT, sizeof(SERVER_ROOT));
  strncat(full_path, path, strlen(path));

  if(full_path[strlen(full_path) - 1] == '/') {
    full_path[strlen(full_path) - 1] = '\0';
  }

  struct stat stat_buf;
  stat(full_path, &stat_buf);

  if(S_ISDIR(stat_buf.st_mode)) {
    char* index = "/index.html";
    strncat(full_path, index, strlen(index));
  }

  printf("full_path: %s\n", full_path);
}

int isFileExist(char* filename) {
  int fd = open(filename, O_RDONLY);
  close(fd);
  if(fd > 0) {
    return 0;
  }
  return 1;
}

void printLine(void) {
  putchar('\n');
  puts("-----------------------------------------------------------------------------------");
  putchar('\n');
}

void error(const char* message) {
  perror(message);
  exit(-1);
}


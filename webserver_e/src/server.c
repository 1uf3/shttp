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

#define SERVER_ROOT "/Users/lufe/develop/system/htdocs"


int isDir(const char* name);
void getRequestFileName(const char* req, char* filename);
int isFileExist(char* filename);
void server_status(int socket, const char* message);

void httpd(int socket);

void error(const char* message);

int main(int argc, char *argv[]) {

  int socket_srv;
  struct sockaddr_in srv;
  socklen_t srv_sock_len;
  int i = 0;

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

  putchar('\n');
  puts("-----------------------------------------------------------------------------------");
  putchar('\n');

  while(1){
    struct sockaddr_in cli;
    socklen_t cli_sock_len;
    int socket_cli;

    cli_sock_len = sizeof(cli);
    if ( (socket_cli = accept(socket_srv, (struct sockaddr *)&cli, &cli_sock_len)) < 0 ) {
      close(socket_srv);
      error("accept");
    }

    int status;
    pthread_t worker;

    if(pthread_create(&worker, NULL, (void *)httpd, (void *)socket_cli ) != 0) {
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
  putchar('\n');
  puts("-----------------------------------------------------------------------------------");
  putchar('\n');

  char file_path[1024];
  memset(file_path, 0, sizeof(file_path));
  char tmp[1024];
  memset(tmp, 0, sizeof(tmp));
  getRequestFileName(request, file_path);
  // printf("%s\n size%lu\n, file_path, strlen(file_path)");
  printf("\n debug \n");
  if(isDir(file_path) == 1) {
    strcpy(tmp, file_path);
    if(tmp[strlen(tmp)-1] != '/') {
      strcat(tmp, "/");
    }
    strcat(tmp, "index.html");
    strcpy(file_path, tmp);
  }
  // printf("%s\n size%lu\n, file_path, strlen(file_path)");

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
  printf("\n debug3 \n");
}

int isDir(const char* name) {
  struct stat* stat_buf;
  stat(name, stat_buf);
  if((stat_buf->st_mode & S_IFMT) == S_IFREG) {
    return 0;
  } else if((stat_buf->st_mode & S_IFMT) == S_IFDIR) {
    return 1;
  } else {
    return -1;
  }
}

void getRequestFileName(const char* req, char* name) {
  char tmp[1024];
  memset(tmp, 0, sizeof(tmp));
  sscanf(req, "%[^\n]", tmp); 

  char *method = strtok(tmp, " ");       
  char *path = strtok(NULL, " ");  

  strcpy(name, SERVER_ROOT);
  strcat(name, path);
}

int isFileExist(char* filename) {
  int fd = open(filename, O_RDONLY);
  close(fd);
  if(fd > 0) {
    return 0;
  }
  return 1;
}

void error(const char* message) {
  perror(message);
  exit(-1);
}


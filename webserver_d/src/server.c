#include <stdio.h> 
#include <netdb.h> 
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define CHILD_NUM 10 
#define SERVER_ROOT "/Users/lufe/develop/system/htdocs"

typedef struct {
  pid_t pid;
  int is_running;
} child_t;

typedef struct {
  child_t child_list[CHILD_NUM];
  int len_child_list;
  int num_running_child;
} process_t;

int sock_srv;

void error(const char* message);
int isDir(const char* name);
int isFileExist(char* filename);
void getRequestFileName(const char* req, char* filename);
void server_status(int socket, const char* message);

void connection_handler();

void add_new_process(pid_t pid, process_t* process_manager);
void make_process(process_t* process_manager);
int check_finish_process(pid_t pid, process_t* process_manager);
void restart_process(int finished_pid_num, process_t* process_manager);

int main(void) {

  process_t process_manager = {{0,0,0},0,0};

  int socket_cli;
  struct sockaddr_in srv;
  socklen_t srv_sock_len;
  struct sockaddr_in cli;
  socklen_t cli_sock_len;
  int i = 0;

  if ( (sock_srv = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    error("socket");
  }

  // Initialize
  memset(&srv, 0, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons(4000);
  srv.sin_addr.s_addr = INADDR_ANY;

  srv_sock_len = sizeof(srv);
  if ( (bind(sock_srv, (struct sockaddr*)&srv, srv_sock_len)) < 0 ) {
    error("bind");
  }

  if( listen(sock_srv, 10) < 0 ) {
    error("listen");
  }

  puts("TCP/IP socket available");
  printf("\taddr %s\n", inet_ntoa(srv.sin_addr));
  printf("\tport %d\n", ntohs(srv.sin_port));

  putchar('\n');
  puts("-----------------------------------------------------------------------------------");
  putchar('\n');

  make_process(&process_manager);
  
  while(1){
  }
  close(sock_srv);
  exit(EXIT_SUCCESS);
}

void error(const char* message) {
  perror(message);
  exit(1);
}

int isDir(const char* name) {
  struct stat* stat_buf;
  stat(name, stat_buf);
  if((stat_buf->st_mode & S_IFMT) == S_IFREG) {
    return 0;
  } else if((stat_buf->st_mode & S_IFMT) == S_IFDIR) {
    return 1;
  } 
  error("stat");
  return EXIT_FAILURE; // 不要だがないとWarning
}

int isFileExist(char* filename) {
  int fd = open(filename, O_RDONLY);
  close(fd);
  if(fd > 0) {
    return 0;
  }
  return 1;
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

void server_status(int socket, const char* message) {
  if(write(socket, message, strlen(message)) < 1) {
    error("write");
  }
}

void add_new_process(pid_t pid, process_t* process_manager){
  int child_number;
  process_manager->len_child_list++;
  process_manager->num_running_child++;

  child_t* child_list = process_manager->child_list;
  child_number = process_manager->len_child_list;

  child_list[child_number].pid = pid;
  child_list[child_number].is_running = 1;
}

void make_process(process_t* process_manager){

  pid_t pid;
  int status;
  int i = 0;

  while(i < 10){
    pid = fork();
    if (pid < 0) {
      error("fork");
    } 
    if (pid == 0) {
      connection_handler();
    }
    if (pid > 0) {
      add_new_process(pid, process_manager);
      while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status) != 0) {
          check_finish_process(pid, process_manager);
        }
      }
    }
    i++;
  }
}

int check_finish_process(pid_t pid, process_t* process_manager) {
  int i;
  int child_number;

  child_t* child_list = process_manager->child_list;
  child_number = process_manager->len_child_list;

  for (i = 0; i < child_number ; i++) {
    if (child_list[i].pid == pid) {
      break;
    }
  }
  child_list[i].is_running = 0;

  restart_process(i, process_manager);

  return EXIT_SUCCESS;
}

void restart_process(int finished_pid_num, process_t* process_manager) {
  pid_t pid;
  pid = fork();
  if (pid < 0) {
    error("fork");
  } 
  if (pid == 0) {
    connection_handler();
  }
  if (pid > 0) {
    child_t* child_list = process_manager->child_list;

    child_list[finished_pid_num].pid = pid;
    child_list[finished_pid_num].is_running = 1;
  }
}

void connection_handler() {
  int socket_cli;
  struct sockaddr_in cli;
  socklen_t cli_sock_len;
  int i = 0;
  while(1){

    cli_sock_len = sizeof(cli);
    if ( (socket_cli = accept(sock_srv, (struct sockaddr *)&cli, &cli_sock_len)) < 0 ) {
      close(sock_srv);
    }

    char buff[8190];
    char filename[1100];
    memset(buff,0,sizeof(buff));
    memset(filename,0,sizeof(filename));
    int req = read(socket_cli, buff, sizeof(buff));
    if (req < 0) {
      error("read");
    }

    // Print http request
    printf("HTTP Request: %s\n", buff);
    putchar('\n');


    putchar('\n');
    puts("-----------------------------------------------------------------------------------");
    putchar('\n');

    getRequestFileName(buff, filename);
    //    printf("%s\nsize%lu\n", filename, strlen(filename));
    char tmp[1024];
    char request[10000];
    if(isDir(filename) == 1) {
      strcpy(tmp, filename);
      if (tmp[strlen(tmp)-1] != '/') {
        strcat(tmp, "/");
      }
      strcat(tmp, "index.html");
      strcpy(filename, tmp);
    }
    //    printf("%s\nsize%lu\n", filename, strlen(filename));

    if(isFileExist(filename) != 0) {
      strcpy(request, "HTTP/1.0 404 NotFound\r\n");
    }

    if(isFileExist(filename) == 0) {
      strcpy(request, "HTTP/1.0 200 OK\r\n\r\n");
      FILE *fp;
      fp = fopen(filename, "r");
      while(1) {
        char tmp[128];
        if(fgets(tmp, 128, fp) == NULL) {
          break;
        }
        strcat(request, tmp);
      }
      fclose(fp);
    }

    putchar('\n');
    puts("-----------------------------------DEBUG-------------------------------------------");
    putchar('\n');
    printf("%s", request);
    putchar('\n');
    puts("-----------------------------------------------------------------------------------");
    putchar('\n');

    server_status(socket_cli, request);

    close(socket_cli);
  }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/select.h>

#define BUFF_SIZE 1500

int main(int argc, char **argv)
{
 
  if (argc != 2){
    printf("Use command: %s <port>\n", argv[0]);// proverka commandi
    exit(0);
  }

  int port = atoi (argv[1]); // лучше использовать strtol
  ssize_t read_len;
  int sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);// socket sozdan
  if (sockfd < 0){
    perror("[-]socket error");
    exit(1);
  }

  struct sockaddr_in server_addr = {//addr_socketa
    .sin_family = AF_INET,
    .sin_port = htons(port),
    .sin_addr = { .s_addr = INADDR_ANY }
  };

  int rc = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (rc < 0) {
    perror("[-]bind error");//socket s addresom
    exit(1);
  }

  fd_set active_set, read_set;
  FD_ZERO(&active_set);
  FD_SET(sockfd,&active_set);
  int i;
  socklen_t size;
};
  
  while (1) {

    

    char buffer[BUFF_SIZE]; // = {0}
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);

    //select()
    int i;
    socklen_t size;
    struct client;

    read_set=active_set;
    if(select(FD_SETSIZE,&read_set,NULL,NULL,NULL)<0){//SELECT WORK
      perror("[-]select error");
      exit(EXIT_FAILURE);
    }
    for(i=0;i<FD_SETSIZE;i++){
      if(FD_ISSET(i,&read_set)){
        if(i==sockfd){
          size=sizeof(client_addr);
          int new_sock = accept(sockfd,(struct sockaddr*)&client_addr,&addr_size);
          if(new_sock<0){
            perror("[-]accept err");
            exit(EXIT_FAILURE);
          }
          printf("[+]Data recv: %s\n", buffer);
          FD_SET(new_sock,&active_set);
        } else { 
            int err = read_len;
            if(err<0){
              close(i);
              FD_CLR(i,active_set);
            } else{ 
              if(strstr(buffer,"stop")){
                close(i);
                FD_CLR(i,active_set);
              }
        } else {
      }
    }
  
    read_len = recvfrom(sockfd, buffer, BUFF_SIZE, 0, (struct sockaddr*)&client_addr, &addr_size);
    if (read_len < 0) {
      if (errno == EINTR) {
        continue;
      }
      perror("[-]recvfrom error");
      exit(1);
    } // chtenie
    buffer[read_len] = '\0';
    printf("[+]Data recv: %s\n", buffer);

    const char *reply = "Server online\n";
    strcpy(buffer, reply);
    ssize_t sent_len = sendto(sockfd, buffer, strlen(reply), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if (sent_len < 0) {
      if (errno == EINTR) {
        // ???
      }
    }
    printf("[+]Data send: %s\n", buffer);// otpravka
  }
  } 
  return 0;
}

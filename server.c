#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/select.h>
//#include <io.h>

#define BUFFLEN 1500



int main(int argc, char **argv){
  if (argc != 2){
    printf("Use command: %s <port>\n", argv[0]);
    exit(0);
  }
  int port = atoi (argv[1]); 
  
  struct sockaddr_in server = {
    .sin_family = AF_INET,
    .sin_port = htons(port),
    .sin_addr = { .s_addr = INADDR_ANY }
  };
  int sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
  if (sock < 0){
    perror("[-]socket error");
    exit(1);
  }
  int err = bind(sock, (struct sockaddr*)&server, sizeof(server));
  if (err < 0) {
    perror("[-]bind error");
    exit(1);
  }

  fd_set active_set, read_set;
  FD_ZERO(&active_set);
  FD_SET(sock,&active_set);
  int i;
  int new_sock;

  while(1){
    
    char buff[BUFFLEN];
    struct sockaddr_in client;
    socklen_t size = sizeof(client);
    read_set=active_set;
    if( select(FD_SETSIZE,&read_set,NULL,NULL,NULL)<0){
      perror("[-]select error");
      exit(EXIT_FAILURE);
    };
    for (i=0;i<FD_SETSIZE;i++){
      if(FD_ISSET(i,&read_set)){
        if(i==sock){
          size = sizeof(client);
          new_sock=accept(sock,(struct sockaddr*)&client,&size);
          if(new_sock<0){
            perror("accept");
            exit(EXIT_FAILURE);
          }
        }
          fprintf(stdout,"server connect from host %s, port %hu.\n",
            inet_ntoa(client.sin_addr),
            ntohs(client.sin_port));
            FD_SET(new_sock,&active_set);
      }else {
        err = recvfrom(sock, buff, BUFFLEN, 0, (struct sockaddr*)&client, &size);
        if (err<0) {
          close(i);
          FD_CLR(i,&active_set);

        }else{
          if(strstr(buff,"stop")){
            close(i);
            FD_CLR(i,&active_set);
          }else{
            const char *reply = "Server msg\n";
            strcpy(buff, reply);
            ssize_t sent_len = sendto(sock, buff, strlen(reply), 0, (struct sockaddr*)&client, sizeof(client));
            printf("Data send: %s\n", buff);
          }
        }
      }
    }
  }
}


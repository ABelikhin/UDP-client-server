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

int main(int argc, char **argv)
{
  if (argc != 2){
    printf("Use command: %s <port>\n", argv[0]);
    exit(0);
  }
  int port = atoi(argv[1]);

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
  FD_SET(sock, &active_set);

  while(1) {
    char buff[BUFFLEN];
    read_set = active_set;
    if (select(sock + 1, &read_set, NULL, NULL, NULL) < 0) {
      perror("[-]select error");
      exit(EXIT_FAILURE);
    }
    if(FD_ISSET(sock, &read_set)) {
      struct sockaddr_in client;
      socklen_t size = sizeof(client);
      // FIXME: BUG with buff
      ssize_t recvlen = recvfrom(sock, buff, BUFFLEN, 0, (struct sockaddr *)&client, &size);
      if (recvlen < 0) {
        if (errno == EAGAIN || errno == EINTR) {
          continue;
        }
        perror("[-]recvfrom error");
        exit(EXIT_FAILURE);
      }
      buff[recvlen] = '\0';
      printf("[+]Data received: %s\n", buff);

      const char *reply = "Server msg\n";
      strcpy(buff, reply);
      ssize_t sent_len = sendto(sock, buff, strlen(reply), 0,
                                (struct sockaddr*)&client, sizeof(client));
      printf("[+]Data sent: %s\n", buff);
    }
  }
}

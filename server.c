#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUFF_SIZE 1500

int main(int argc, char **argv)
{
  if (argc != 2){
    printf("Use command: %s <port>\n", argv[0]);
    exit(0);
  }

  int port = atoi(argv[1]); // лучше использовать strtol

  int sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
  if (sockfd < 0){
    perror("[-]socket error");
    exit(1);
  }

  struct sockaddr_in server_addr = {
    .sin_family = AF_INET,
    .sin_port = htons(port),
    .sin_addr = { .s_addr = INADDR_ANY }
  };

  int rc = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (rc < 0) {
    perror("[-]bind error");
    exit(1);
  }

  while(1) {
    char buffer[BUFF_SIZE]; // = {0}
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);

    select(); //
    ssize_t read_len = recvfrom(sockfd, buffer, BUFF_SIZE, 0, (struct sockaddr*)&client_addr, &addr_size);
    if (read_len < 0) {
      if (errno == EINTR) {
        continue;
      }
      perror("[-]recvfrom error");
      exit(1);
    }
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
    printf("[+]Data send: %s\n", buffer);
  }
  return 0;
}

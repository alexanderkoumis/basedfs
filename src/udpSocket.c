// Guide: https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h> // socketaddr_in
#include <stdio.h> // fprintf()
#include <string.h> // memcpy()
#include <errno.h>


int main(int argc, char** argv) {
  struct hostent* hp;
  int fd;
  printf("fuck u bitch");
  perror("fuck u bitch2");
  fprintf(stderr, "fuck!");
  struct sockaddr_in serverIn;
  char* host = "127.0.0.1";
  char* msg = "test msg";
  perror("fuck you!");
  memset((char*)&serverIn, 0, sizeof(serverIn));
  serverIn.sin_family = AF_INET;
  serverIn.sin_port = htons(1337);
  hp = gethostbyname(host);
  perror("fuck nigga!!");
  if (!hp) {
    fprintf(stderr, "whoops shit %s\n", host);
    return 0;
  }
  memcpy((void*)&serverIn.sin_addr, hp->h_addr_list[0], hp->h_length);
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("cannot create socket\n");
                return 0;
        }
  if (sendto(fd, msg, strlen(msg), 0, (struct sockaddr*)&serverIn, sizeof(serverIn)) < 0) {
    perror("sendto fuck!");
  }
  return 0;
}

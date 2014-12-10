#include <stdio.h> // printf(), perror()
#include <arpa/inet.h> // sockaddr_in, socket()
#include <string.h>

const static int bufferSize = 4096;

int main(int argc, char** argv) {

  int sock = 0;
  int rec = 0;
  int recLen = 0;
  int sendLen = 0;
  struct sockaddr_in recSocket;

  struct sockaddr_in sendSocket;

  char oBuffer[bufferSize];
  char iBuffer[bufferSize];

  // Create UDP socket
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    perror("What the fuck!!!! Error!");
    return -1;
  }

  // User process address
  memset(&recSocket, 0, sizeof(recSocket));
  recSocket.sin_family = AF_INET;
  recSocket.sin_addr.s_addr = inet_addr("127.0.0.1");
  recSocket.sin_port = htons(5003);

  recLen = sizeof(recSocket);
  if (bind(sock, (struct sockaddr *) &recSocket, recLen) < 0) {
    perror("Damn!!! bind fucked up!!");
    return -1;
  }

  // Kernel process address
  memset(&sendSocket, 0, sizeof(sendSocket));
  sendSocket.sin_family = AF_INET;
  sendSocket.sin_addr.s_addr = inet_addr("127.0.0.1");
  sendSocket.sin_port = htons(5002);

  char msg[] = "fuck u!!!!";
  memcpy(oBuffer, msg, strlen(msg) + 1);
  sendLen = strlen(oBuffer) + 1;

  if (sendto(sock, oBuffer, sendLen, 0, (struct sockaddr*) &sendSocket,
    sizeof(sendSocket)) != sendLen) {
    perror("OHHHHHH SHIT!!!!!!!! sendto(sock,buf...)");
    return -1;
  };

  printf("oBuffer: %s\niBuffer: %s\n", oBuffer, iBuffer);

  memset(iBuffer, 0, bufferSize);
  printf("sock: %i\n", sock);
  if ((rec = recvfrom(sock, iBuffer, bufferSize, 0, NULL, NULL)) < 0) {
    perror("SOMETHINGS MESSED UP IN RECVDDDDD");
    return -1;
  }

  while (1) {
    printf("oBuffer: %s\niBuffer: %s\n", oBuffer, iBuffer);
    printf("ok now we wait for the package from server\n");
    if ((rec = recvfrom(sock, iBuffer, bufferSize, 0, NULL, NULL)) < 0) {
      perror("SOMETHINGS MESSED UP IN RECVDDDDD");
      return -1;
    }
    printf("oBuffer: %s\niBuffer: %s\n", oBuffer, iBuffer);
  }

  return 0;  
}
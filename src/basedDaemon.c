#include <stdio.h> // printf(), perror()
#include <arpa/inet.h> // sockaddr_in, socket()
#include <string.h>


const static int bufferSize = 4096;

int main(int argc, char** argv) {

  int sock = 0;
  int rec = 0;
  int recLen = 0;
  int sendLen = 0;

  int serverPort = 5002;
  int clientPort = 5003;

  struct sockaddr_in recSocket;
  struct sockaddr_in sendSocket;

  char oBuffer[bufferSize];
  char iBuffer[bufferSize];
  char nameBuffer[bufferSize];
  char dataBuffer[bufferSize];
  char prefix[] = "/home/ubuntu/fs/";

  // Create UDP socket
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    perror("What the fuck!!!! Error!");
    return -1;
  }

  // User process address
  memset(&recSocket, 0, sizeof(recSocket));
  recSocket.sin_family = AF_INET;
  recSocket.sin_addr.s_addr = inet_addr("127.0.0.1");
  recSocket.sin_port = htons(clientPort);

  recLen = sizeof(recSocket);
  if (bind(sock, (struct sockaddr *) &recSocket, recLen) < 0) {
    perror("Damn!!! bind fucked up!!");
    return -1;
  }

  // Kernel process address
  memset(&sendSocket, 0, sizeof(sendSocket));
  sendSocket.sin_family = AF_INET;
  sendSocket.sin_addr.s_addr = inet_addr("127.0.0.1");
  sendSocket.sin_port = htons(serverPort);

  const char n = '\n';

  while (1) {
    char* op = NULL;
    char* arg1 = NULL;
    char* arg2 = NULL;
    printf("Listening for basedfs packages on port %d\n", clientPort);
    memset(iBuffer, 0, bufferSize);
    if ((rec = recvfrom(sock, iBuffer, bufferSize, 0, NULL, NULL)) < 0) {
      perror("SOMETHINGS MESSED UP IN RECVDDDDD");
      return -1;
    }
    op = strtok(iBuffer, &n);
    arg1 = strtok(NULL, &n);
    arg2 = strtok(NULL, &n);
    printf("\tOp: %s\n", op);
    printf("\tArg1: %s\n", arg1);
    if (arg2) {
      printf("\tArg2: %s\n", arg2);
    }
    if (strstr(op, "open")) {
      memset(nameBuffer, 0, bufferSize);
      strcpy(nameBuffer, prefix);
      strcat(nameBuffer, arg1);
      printf("\t\tCreating file: %s\n", nameBuffer);
      FILE* infile;
      infile = fopen(nameBuffer, "w");
      printf("\t\tClosing file: %s\n", nameBuffer);
      fclose(infile);
    }
    else if (strstr(op, "read")) {
      memset(nameBuffer, 0, bufferSize);
      strcpy(nameBuffer, prefix);
      strcat(nameBuffer, arg1);
      printf("\t\tOpening file: %s\n", nameBuffer);
      FILE* infile;
      infile = fopen(nameBuffer, "r");
      printf("\t\tClosing file: %s\n", nameBuffer);
      fclose(infile);
    }
    else if (strstr(op, "write")) {
      memset(nameBuffer, 0, bufferSize);
      strcpy(nameBuffer, prefix);
      strcat(nameBuffer, arg1);
      arg2 = strtok(NULL, &n);
      printf("\t\tOpening file: %s\n", nameBuffer);
      FILE* infile;
      infile = fopen(nameBuffer, "w");
      printf("\t\tClosing file: %s\n", nameBuffer);
      fclose(infile);
    }
    memset(iBuffer, 0, bufferSize);
  }

  return 0;  
}




  // printf("sock: %i\n", sock);
  // if ((rec = recvfrom(sock, iBuffer, bufferSize, 0, NULL, NULL)) < 0) {
  //   perror("SOMETHINGS MESSED UP IN RECVDDDDD");
  //   return -1;
  // }

  // char msg[] = "fuck u!!!!";
  // memcpy(oBuffer, msg, strlen(msg) + 1);
  // sendLen = strlen(oBuffer) + 1;

  // if (sendto(sock, oBuffer, sendLen, 0, (struct sockaddr*) &sendSocket,
  //   sizeof(sendSocket)) != sendLen) {
  //   perror("OHHHHHH SHIT!!!!!!!! sendto(sock,buf...)");
  //   return -1;
  // };

  // printf("oBuffer: %s\niBuffer: %s\n", oBuffer, iBuffer);

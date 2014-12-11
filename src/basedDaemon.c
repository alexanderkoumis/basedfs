#include <stdio.h> // printf(), perror()
#include <arpa/inet.h> // sockaddr_in, socket()
#include <string.h>


const static int bufferSize = 4096;

void createDummyFile() {
	FILE* dummyFile;
	char* dummyText = "This is some text";
	dummyFile = fopen("/home/ubuntu/fs/dummy", "w");
	fprintf(dummyFile, "%s", dummyText);
	printf("Just printed \"%s\" to the dummy file.\n", dummyText);
	fclose(dummyFile);
}

int main(int argc, char** argv) {

  createDummyFile();
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

    printf("Listening for basedfs packages on port %d\n", clientPort);

    char* op = NULL;
    char* fid = NULL;
    char* payload = NULL;

    memset(iBuffer, 0, bufferSize);
    memset(oBuffer, 0, bufferSize);

    if ((rec = recvfrom(sock, iBuffer, bufferSize, 0, NULL, NULL)) < 0) {
      perror("SOMETHINGS MESSED UP IN RECVDDDDD");
      return -1;
    }

    op = strtok(iBuffer, &n);
    fid = strtok(NULL, &n);
    payload = strtok(NULL, &n);

    printf("\tOp: %s\n", op);
    printf("\tFID: %s\n", fid);
    if (payload) {
      printf("\tPayload: %s\n", payload);
    }

    memset(nameBuffer, 0, bufferSize);
    strcpy(nameBuffer, prefix);
    strcat(nameBuffer, fid);

    if (strstr(op, "open")) {
      printf("\t\tCreating file: %s\n", nameBuffer);
      FILE* infile;
      infile = fopen(nameBuffer, "w");

      printf("\t\tClosing file: %s\n", nameBuffer);
      fclose(infile);
    }

    else if (strstr(op, "read")) {
      printf("\t\tOpening file: %s\n", nameBuffer);

      FILE* infile;
      infile = fopen(nameBuffer, "r+");
      fread(oBuffer, sizeof(char), bufferSize, infile);

      if (sendto(sock, oBuffer, sendLen, 0, (struct sockaddr*) &sendSocket,
        sizeof(sendSocket)) != sendLen) {
        perror("OHHHHHH SHIT!!!!!!!! sendto(sock,buf...)");
        return -1;
      };

      printf("\t\t\tFile contents: %s\n", oBuffer);
      printf("\t\tClosing file: %s\n", nameBuffer);
      fclose(infile);
    }

    else if (strstr(op, "write")) {
      printf("\t\tOpening file: %s\n", nameBuffer);
      FILE* infile;
      infile = fopen(nameBuffer, "w");
      fprintf(infile, "%s", payload);
      printf("\t\t\tWrote \"%s\" to file:\n\t\t\t\t%s\n", payload, nameBuffer);

      printf("\t\tClosing file: %s\n", nameBuffer);
      fclose(infile);

      sprintf(oBuffer, "%s\n%lx\n%s", op, (unsigned long)fid, "");
      printf("\t\tSending \"%s\" back to kernel\n", oBuffer);
      if (sendto(sock, oBuffer, 512, 0, (struct sockaddr*) &sendSocket,
        sizeof(sendSocket)) != 512) {
        perror("OHHHHHH SHIT!!!!!!!! sendto(sock,buf...)");
        return -1;
      };

    }
  }

  return 0;
}


  // if (sendto(sock, oBuffer, sendLen, 0, (struct sockaddr*) &sendSocket,
  //   sizeof(sendSocket)) != sendLen) {
  //   perror("OHHHHHH SHIT!!!!!!!! sendto(sock,buf...)");
  //   return -1;
  // };




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

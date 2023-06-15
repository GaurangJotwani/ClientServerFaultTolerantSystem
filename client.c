/*
 * client.c -- TCP Socket Client
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */
#include "client.h"

// Global Variable to hold IP_address
char ip_addr[16];

// Global Variable to hold Port Number
int port;

// Global Variable to hold socket descriptor
int socket_desc;

int main(int argc, char* argv[]) {
  read_config("config.txt");
	int socket_desc;
	struct sockaddr_in server_addr;
	char server_message[8196], client_message[8196];

  // Clean buffers:
	memset(server_message,'\0',sizeof(server_message));
	memset(client_message,'\0',sizeof(client_message));

  // Create socket:
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	if(socket_desc < 0){
		printf("Unable to create socket\n");
		return -1;
	}

	printf("Socket created successfully\n");

  // Set port and IP the same as server-side:
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip_addr);

  // Send connection request to server:
	if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		printf("Unable to connect\n");
		return -1;
	}
	printf("Connected with server successfully\n");

	// Check first argument to know what type of request to send
  if (strcmp(argv[1], "GET") == 0 && (argc == 3 || argc == 4)) {
    if (argc == 3) {
      getRequest(socket_desc, argv[2], argv[2]);
    } else {
      getRequest(socket_desc, argv[2], argv[3]);
    }
  } else if(strcmp(argv[1], "INFO") == 0 && argc == 3) {
    infoRequest(socket_desc, argv[2]);
  } else if(strcmp(argv[1], "MD") == 0 && argc == 3) {
    mdRequest(socket_desc, argv[2]);
  } else if(strcmp(argv[1], "RM") == 0 && argc == 3) {
    rmRequest(socket_desc, argv[2]);
  } else if(strcmp(argv[1], "PUT") == 0 && (argc == 3 || argc == 4)) {
    if (argc == 3) {
      putRequest(socket_desc, argv[2], argv[2]);
    } else {
      putRequest(socket_desc, argv[2], argv[3]);
    }
  } else {
    printf("Wrong number of arguments\n");
  }
  // Close the socket:
  close(socket_desc);
  
  return 0;
}

char* concTwoString(char* str1, char* str2) {
  char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2));
  strcpy(str3, str1);
  return strcat(str3, str2);
}

void getRequest(int socket_desc, char* remote_file, char* local_file) {
  char* str1 = "GET:";
  char str3[8196];
  memset(str3, '\0', sizeof(str3));
  strcpy(str3, concTwoString(str1, remote_file));
  if(send(socket_desc, str3, strlen(str3), 0) < 0){
    printf("Unable to send message\n");
    return;
  }
  FILE* fp = fopen(local_file, "wb");
  char server_message[8];
  while(1) {
    int n = recv(socket_desc, server_message, sizeof(server_message), 0);
    if (n <= 0){
      break;
      return;
    }
    fprintf(fp, "%s", server_message);
    
    bzero(server_message, sizeof(server_message));
  }
  fclose(fp); 
}

void infoRequest(int socket_desc, char* remote_file) {
  char* str1 = "INFO:";
  char str3[8196];
  memset(str3, '\0', sizeof(str3));
  strcpy(str3, concTwoString(str1, remote_file));
  if(send(socket_desc, str3, strlen(str3), 0) < 0){
    printf("Unable to send message\n");
    return;
  }
  char server_message[8196];
  memset(server_message, '\0', sizeof(server_message));
  if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
    printf("Error while receiving server's msg\n");
    return;
  }
  printf("Server's response: %s\n",server_message);
}

void mdRequest(int socket_desc, char* remote_file) {
  char* str1 = "MD:";
  char str3[8196];
  memset(str3, '\0', sizeof(str3));
  strcpy(str3, concTwoString(str1, remote_file));
  if(send(socket_desc, str3, strlen(str3), 0) < 0){
    printf("Unable to send message\n");
    return;
  }
  char server_message[8196];
  memset(server_message, '\0', sizeof(server_message));
  if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
    printf("Error while receiving server's msg\n");
    return;
  }
  printf("Server's response: %s\n",server_message);
}

void rmRequest(int socket_desc, char* remote_file) {
  char* str1 = "RM:";
  char str3[8196];
  memset(str3, '\0', sizeof(str3));
  strcpy(str3, concTwoString(str1, remote_file));
  if(send(socket_desc, str3, strlen(str3), 0) < 0){
    printf("Unable to send message\n");
    return;
  }
  char server_message[8196];
  if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
    printf("Error while receiving server's msg\n");
    return;
  }
  printf("Server's response: %s\n",server_message);
}

void putRequest(int socket_desc, char* remote_file, char* local_file) {
  char* str1 = "PUT:";
  char str3[8196];
  memset(str3, '\0', sizeof(str3));
  strcpy(str3, concTwoString(str1, remote_file));
  printf("%s\n", str3);
  if(send(socket_desc, str3, strlen(str3), 0) < 0) {
    printf("Unable to send message\n");
    return;
  }
  usleep(90000);
  char server_message[8];
  memset(server_message, '\0', sizeof(server_message));
  FILE* fp = fopen(local_file, "rb");
  while(fgets(server_message, sizeof(server_message), fp) != NULL) {
    if (send(socket_desc, server_message, sizeof(server_message), 0) == -1) {
      printf("Error in sending file in PUT\n");
      return;
    }
    bzero(server_message, sizeof(server_message));
  }
  fclose(fp); 
}

void read_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening config file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (strcmp(key, "IP_ADDR") == 0) {
            strcpy(ip_addr, value);
        }  else if (strcmp(key, "PORT") == 0) {
            port = atoi(value);
        }
    }
    fclose(file);
}

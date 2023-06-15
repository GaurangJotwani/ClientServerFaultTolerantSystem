  /*
  * server.c -- TCP Socket Server
  * 
  * adapted from: 
  *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
  */

#include "server.h"

// Global Variable to hold IP_address
char ip_addr[16];
// Global Variable to hold disk_1 path
char disk_1[256];

// Global Variable to hold disk_2 path
char disk_2[256];

// Global Variable to hold port number
int port;

// Global array to hold disk paths for both disks
char* disk_paths[] = {disk_1, disk_2};

// Global array to hold availability of both the disk. 1 is available
int availability[] = {1, 1};

// Global array to hold the disk which is currently the master
char* master_disk;

// Global array to keep track of weather the disks are being scanned for consistency.
int is_scanning = 0;

int main(void) {
  read_config("config.txt");
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if(socket_desc < 0){
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");

  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip_addr);

  // Bind to the set port and IP:
  if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    printf("Couldn't bind to the port\n");
    return -1;
  }
  printf("Done with binding\n");

  // Listen for clients:
  if(listen(socket_desc, 1) < 0){
    printf("Error while listening\n");
    return -1;
  }
  printf("\nListening for incoming connections.....\n");

  while(1) {
    // Accept an incoming connection: 
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    
    if (client_sock < 0){
      printf("Can't accept\n");
      return -1;
    }
    printf("Client connected at IP: %s and port: %i\n", 
     inet_ntoa(client_addr.sin_addr), 
     ntohs(client_addr.sin_port));

    if (access(disk_1, F_OK) != -1 && access(disk_2, F_OK) != -1) {
      master_disk = disk_1;
      availability[1] = 1;
      availability[0] = 1;
      scan();
    } else if (access(disk_1, F_OK) != -1) {
      master_disk = disk_1;
      availability[0] = 1;
      availability[1] = 0;
    } else {
      master_disk = disk_2;
      availability[1] = 1;
      availability[0] = 0;
    }

    pid_t pid = fork();
    if (pid == 0) {
      printf("Created process ID: %d\n", pid);
      process_request(client_sock);  
    }

    usleep(50000);
    close(client_sock);
  }

  // Closing the socket:
  close(socket_desc);

  return 0;
}

int handle_get(int client_sock, char* file_path) {
  printf("%s\n", file_path);
  char server_message[8];
  memset(server_message, '\0', sizeof(server_message));

  char* final_path = concTwoString(master_disk, file_path);
  printf("%s\n", final_path);

  FILE* fp = fopen(final_path, "rb");
  while(fgets(server_message, 8, fp) != NULL) {
    if (send(client_sock, server_message, sizeof(server_message), 0) == -1) {
      printf("Error in sending file in GET\n");
      return -1;
    }
    bzero(server_message, sizeof(server_message));
  }

  fclose(fp);
  return 0;
}

int handle_info(char* server_message, char* file_path) {
  char* final_path = concTwoString(master_disk, file_path);
  printf("%s\n", final_path);

  int file_desc = open(final_path, O_RDWR);
  struct stat info_st;
  if (fstat(file_desc, &info_st) != 0) {
    printf("Error getting file information INFO\n");
    return -1;
  }

  char file_info[8196];
  memset(file_info, '\0', sizeof(file_info));

  struct tm ts;
  char buf[80];

  // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
  ts = *localtime(&info_st.st_mtime);
  strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);

  sprintf(server_message, "\nProtection, Owner User ID, Owner Group ID, Links, Size, Last Modified, Name\n%3o, %u, %u, %ld, %lld, %s, %s\n", info_st.st_mode & PERMISSIONS_MASK, info_st.st_uid, info_st.st_gid, info_st.st_nlink, (long long) info_st.st_size, buf, file_path);
  return 0;
}


int handle_md(char* dir_path) {
  printf("%s\n", dir_path);
  for (int i = 0; i < 2; i++) {
    if (availability[i]) {
      char* final_path = concTwoString(disk_paths[i], dir_path);
      printf("%s\n", final_path);
      struct stat st;
      if (stat(final_path, &st) == -1) {
        if(mkdir(final_path, 0755)) {
          printf("Unable to create directory MD\n");
          return -2;
        } else {
          continue;
        }
      } else {
        if (is_scanning) continue;
        printf("Directory already exists MD\n");
        return -1;
      }
    } else {
      char* str1 = "MD:";
      write_to_backlog(str1, dir_path);
    }
  }
  return 0;
}

int handle_rm(char *path) {
  struct stat path_stat;
  for (int i = 0; i < 2; i++) {
    if (availability[i]) {
      char* final_path = concTwoString(disk_paths[i], path);
      printf("%s\n", final_path);
      if(stat(final_path, &path_stat) == 0) {
        if (path_stat.st_mode & S_IFDIR) {
          if(rmdir(final_path) && !is_scanning)return -1;
        } else if (path_stat.st_mode & S_IFREG) {
          if(remove(final_path) && !is_scanning) return -1;
        }
      }
    } else {
      char* str1 = "RM:";
      write_to_backlog(str1, path);
    }
  }
  return 0;
}

int handle_put(int client_sock, char* local_file) {
  char* path1 = concTwoString(disk_paths[0], local_file);
  char* path2 = concTwoString(disk_paths[1], local_file);
  FILE *fp0, *fp1;

  printf("Disk1 Avai: %d, Disk2 Avai: %d\n", availability[0], availability[1]);
  if (availability[0]) {
    fp0 = fopen(path1, "wb");
  } else {
    char* str1 = "PUT:01:";
    write_to_backlog(str1, local_file);
  }
  if (availability[1]) {
    fp1 = fopen(path2, "wb");
  } else {
    char* str1 = "PUT:02:";
    write_to_backlog(str1, local_file);
  }
  char client_message[8];
  memset(client_message, '\0', sizeof(client_message));
  while(1) {
    int n = recv(client_sock, client_message, sizeof(client_message), 0);
    if (n <= 0) {
      break;
      return n;
    }
    if (availability[0]) fprintf(fp0, "%s", client_message);
    if (availability[1]) fprintf(fp1, "%s", client_message);
    bzero(client_message, sizeof(client_message));
  }
  if (availability[0]) fclose(fp0);
  if (availability[1]) fclose(fp1);
  return 0;
}

int process_request(int client_sock) {
  char client_message[8196];
  char server_message[8196];

  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));

  // Receive client's message:
  if (recv(client_sock, client_message, 
   sizeof(client_message), 0) < 0){
    printf("Couldn't receive\n");
}

printf("Msg from client: %s\n", client_message);
char* client_message_split[2];
char* token = strtok(client_message, ":");
int i = 0;

while(token != NULL) {
  client_message_split[i++] = token;
  token = strtok(NULL, ":");
}

int res = -1;

if (strcmp(client_message_split[0], "GET") == 0) {
  res = handle_get(client_sock, client_message_split[1]);
} else if (strcmp(client_message_split[0], "INFO") == 0) {
  res = handle_info(server_message, client_message_split[1]);
} else if (strcmp(client_message_split[0], "MD") == 0) {
  res = handle_md(client_message_split[1]);
  if (res == 0) {
    sprintf(server_message, "Directory %s created successfully.", client_message_split[1]);
  }
} else if (strcmp(client_message_split[0], "PUT") == 0) {
  res = handle_put(client_sock, client_message_split[1]);
  if (res == 0) {
    sprintf(server_message, "File %s created successfully.", client_message_split[1]);
  }
} else if (strcmp(client_message_split[0], "RM") == 0) {
  res = handle_rm(client_message_split[1]);
  if (res == 0) {
    sprintf(server_message, "%s deleted successfully.", client_message_split[1]);
  }
} else if (strcmp(client_message_split[0], "Q") == 0) {
  sprintf(server_message, "Goodbye\n");
} else {
  printf("Invalid command\n");
}

if (res < 0) {
  sprintf(server_message, "Error in %s.\n", client_message_split[0]);
}

if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
  printf("Can't send response\n");
}
}

void scan() {
  is_scanning = 1;
  const char* file_path = "backlog.txt";
  FILE* file = fopen(file_path, "r");
  if (!file) {
    printf("Error: failed to open file\n");
    return;
  }
  char buffer[1024];
  memset(buffer, '\0', sizeof(buffer));
  while (fgets(buffer, sizeof(buffer), file)) {
    int length = strlen(buffer);
    char* client_message_split[3];
    char* token = strtok(buffer, ":");
    int i = 0;
    buffer[length - 1] = '\0';
    while(token != NULL) {
      client_message_split[i++] = token;
      token = strtok(NULL, ":");
    }
    if (strcmp(client_message_split[0], "MD") == 0) {
      handle_md(client_message_split[1]);
    } else if (strcmp(client_message_split[0], "RM") == 0) {
      handle_rm(client_message_split[1]);
    } else if (strcmp(client_message_split[0], "PUT") == 0) {
      char* path1 = concTwoString(disk_paths[0], client_message_split[2]);
      char* path2 = concTwoString(disk_paths[1], client_message_split[2]);
      if (strcmp(client_message_split[1], "01") == 0) {
        copy_file(path2, path1);
      } else if (strcmp(client_message_split[1], "02") == 0) {
        copy_file(path1, path2);
      }
    }
    bzero(buffer, sizeof(buffer));
  }
  is_scanning = 0;
  fclose(file);
  file = fopen(file_path, "w");
  if (!file) {
    printf("Error: failed to open file\n");
    return;
  }
  fclose(file);
}

int copy_file(const char* src_file_path, const char* dest_file_path) {
    // Open the source file for reading
  FILE* src_file = fopen(src_file_path, "rb");
  if (!src_file) {
    printf("Error: failed to open source file\n");
    return 1;
  }

    // Open the destination file for writing
  FILE* dest_file = fopen(dest_file_path, "wb");
  if (!dest_file) {
    printf("Error: failed to open destination file\n");
    fclose(src_file);
    return 1;
  }

    // Copy the contents of the source file to the destination file
  char buffer[1024];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
    if (fwrite(buffer, 1, bytes_read, dest_file) != bytes_read) {
      printf("Error: failed to write to destination file\n");
      fclose(src_file);
      fclose(dest_file);
      return 1;
    }
  }
    // Close the files
  fclose(src_file);
  fclose(dest_file);

  return 0;
}

char* concTwoString(char* str1, char* str2) {
  char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2));
  strcpy(str3, str1);
  return strcat(str3, str2);
}

void write_to_backlog(char* str1, char* file_path) {
  printf("Writing to backlog - %s: %s\n", str1, file_path);
  const char* backlog_path = "backlog.txt";
  FILE* backlog_file = fopen(backlog_path, "a");
  if (!backlog_file) {
    printf("Error: failed to open backlog file\n");
    return;
  }
  char* str3 = concTwoString(str1, file_path);
  fprintf(backlog_file, "%s\n", str3);
  fclose(backlog_file);
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
        } else if (strcmp(key, "DISK_1") == 0) {
            strcpy(disk_1, value);
        } else if (strcmp(key, "DISK_2") == 0) {
            strcpy(disk_2, value);
        } else if (strcmp(key, "PORT") == 0) {
            port = atoi(value);
        }
    }
    fclose(file);
}

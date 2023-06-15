#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>

#define PERMISSIONS_MASK 0777

/*
 * Function: copy_file
 * ----------------------------
 * Description: Copies the contents of one file to another.
 * Arguments:
 *   - src_file_path: the source file path
 *   - dest_file_path: the destination file path
 * Return: 0 on success, 1 on error
 */
int copy_file(const char* src_file_path, const char* dest_file_path);

/*
 * Function: scan
 * ----------------------------
 * Description: Scans the backlog file and handles the pending requests.
 * Arguments: None
 * Return: None
 */
void scan();


/*
 * Function: concTwoString
 * ----------------------------
 * Description: Concatenates two strings.
 * Arguments:
 *   - str1: the first string
 *   - str2: the second string
 * Return: A new string which is the concatenation of str1 and str2
 */
char* concTwoString(char* str1, char* str2);


/*
 * Function: handle_get
 * ----------------------------
 * Description: Sends the requested file to the client.
 * Arguments:
 *   - client_sock: the client socket descriptor
 *   - file_path: the path of the requested file
 * Return: 0 on success, -1 on error
 */
int handle_get(int client_sock, char* file_path);

/*
 * Function: handle_info
 * ----------------------------
 * Description: Retrieves the file information and stores it in server_message.
 * Arguments:
 *   - server_message: the buffer to store the file information
 *   - file_path: the path of the requested file
 * Return: 0 on success, -1 on error
 */
int handle_info(char* server_message, char* file_path);


/*
 * Function: write_to_backlog
 * ----------------------------
 * Description: Writes a request to the backlog file.
 * Arguments:
 *   - str1: the request type (MD, RM, PUT)
 *   - file_path: the file path related to the request
 * Return: None
 */
void write_to_backlog(char* str1, char* file_path);

/*
 * Function: handle_md
 * ----------------------------
 * Description: Creates a new directory at the specified path.
 * Arguments:
 *   - dir_path: the path where the new directory will be created
 * Return: 0 on success, -1 if the directory already exists, -2 on other errors
 */
int handle_md(char* dir_path);

/*
 * Function: handle_rm
 * ----------------------------
 * Description: Removes the file or directory at the specified path.
 * Arguments:
 *   - path: the path of the file or directory to be removed
 * Return: 0 on success, -1 on error
 */
int handle_rm(char *path);

/*
 * Function: handle_put
 * ----------------------------
 * Description: Receives a file from the client and stores it on the server.
 * Arguments:
 *   - client_sock: the client socket descriptor
 *   - local_file: the path where the received file will be stored
 * Return: 0 on success, non-zero on error
 */
int handle_put(int client_sock, char* local_file);

/*
 * Function: process_request
 * ----------------------------
 * Description: Processes client requests such as GET, INFO, MD, PUT, RM.
 * Arguments:
 *   - client_sock: the client socket descriptor
 * Return: None
 */
int process_request(int client_sock);

/**
 * Reads the configuration from a file and stores the values in the provided variables.
 *
 * This function reads the configuration from the specified file and populates
 * the provided variables with the corresponding values. The configuration file
 * should have key-value pairs in the format "KEY=VALUE", one per line.
 *
 * @param[in] filename The path to the config file.
 * Return: None
 */
void read_config(const char *filename);

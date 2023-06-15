#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * infoRequest - Sends an INFO request to the server
 * @socket_desc: the socket descriptor
 * @remote_file: the name of the remote file
 */
void infoRequest(int socket_desc, char* remote_file);

/**
 * mdRequest - Sends a MD (Make Directory) request to the server
 * @socket_desc: the socket descriptor
 * @remote_file: the name of the remote directory
 */
void mdRequest(int socket_desc, char* remote_file);

/**
 * rmRequest - Sends a RM (Remove) request to the server
 * @socket_desc: the socket descriptor
 * @remote_file: the name of the remote file
 */
void rmRequest(int socket_desc, char* remote_file);

/**
 * putRequest - Sends a PUT request to the server
 * @socket_desc: the socket descriptor
 * @remote_file: the name of the remote file
 * @local_file: the name of the local file
 */

void putRequest(int socket_desc, char* remote_file, char* local_file);

/**
 * getRequest - Sends a GET request to the server
 * @socket_desc: the socket descriptor
 * @remote_file: the name of the remote file
 * @local_file: the name of the local file
 */
void getRequest(int socket_desc, char* remote_file, char* local_file);

/**
 * concTwoString - Helper function that Concatenates two strings
 * @str1: first string
 * @str2: second string
 * 
 * Return: pointer to the newly allocated concatenated string
 */
char* concTwoString(char* str1, char* str2);


/**
 * Reads the configuration from a file and stores the values in the provided variables.
 *
 * This function reads the configuration from the specified file and populates
 * the provided variables with the corresponding values. The configuration file
 * should have key-value pairs in the format "KEY=VALUE", one per line.
 *
 * @param[in] filename The path to the config file.
 * @return void
 */
void read_config(const char *filename);
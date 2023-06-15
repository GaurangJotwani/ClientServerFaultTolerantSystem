# Client-Server Fault Tolerant File System

This project implements a server-based file storage system that allows any program to open a TCP connection through a socket, request file services from a server (reading a file, writing to a file, creating a folder, get information about a file or folder, and deleting a file or folder).


## Getting Started

1. Clone this repository.
2. Update the config.txt file with desired IP_ADRR, PORT, DISK_1 Path and DISK_2 path
3. Compile the server and client code with make command.
3. Run the server in one terminal window with `./server`.
4. Run the client in another terminal window with the desired command, e.g., `./fget GET folder/foo.txt data/localfoo.txt`.

## Usage

### Client

The client program accepts command-line arguments to perform different file operations:

- Read a file from the server: `./fget GET remote-file-path local-file-path`
- Get file information: `./fget INFO remote-file-path`
- Create a folder: `./fget MD remote-folder-path`
- Write a file to the server: `./fget PUT local-file-path remote-file-path`
- Delete a file or folder from the server: `./fget RM remote-file-path`

### Server

The server listens for incoming client connections and processes their requests. The server keeps running continuously until its process is killed.

## Features

1. Read a file from the server's file storage area (GET)
2. Get file information, such as ownership, date of last modification, permissions, size, etc. (INFO)
3. Create a folder on the server (MD)
4. Write a new file to the server (PUT)
5. Delete a file or folder from the server (RM)
6. Mirrored file system: write data to two USB devices simultaneously
7. Multi-processing has been implemented by using fork commands to server side to handle multiple client connections simultaneously (bonus)

## Mirroring

Mirroring in this project is achieved by writing data to two USB devices simultaneously, creating a simple mirrored file system. This approach ensures fault tolerance by providing redundancy in case one of the USB devices fails or becomes unavailable.

### Implementation

When a write operation (PUT or MD or RM) is initiated by the client, the server writes the data to both USB devices (referred to as DISK_1 and DISK_2) in parallel. The server maintains separate file paths for both devices and updates both locations accordingly.

In case one of the USB devices is removed or becomes unavailable, the server continues to serve files and write data to the remaining USB device. In the mean time the server writes the commands to backlog.txt file. When the previously unavailable USB device is reconnected, the server synchronizes the data between the two devices to maintain consistency by reading and executing the commands from backlog.txt.

To handle the reconnection and synchronization of USB devices, the server periodically checks the availability of both devices. If one device is found to be unavailable, it is marked as "unavailable." When the device becomes available again, the server initiates a synchronization process using the scan() function. During this process, the server executed the commands written in backlog.txt

## Testing

There are three test scripts. Run test_general first to test if all the command works and mirrorring is being implemented correctly. Then Run test_mirrorring to check if server is behaving correctly when one of disk is not present. Finally run test_concurrency to check if server can handle multiple simulataneous request.


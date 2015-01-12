/*
 * FileUploader.h
 *
 *  Created on: Sep 24, 2014
 *      Author: vivekanandh
 */

#ifndef FILEUPLOADER_H_
#define FILEUPLOADER_H_

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PACKET_SIZE 1000

typedef struct{
	int  socket_id;
	int  connection_id;
	char host_name[100];
	char IP[INET_ADDRSTRLEN];
	char Port[10];
}IPandPort;

//Declaring variables
struct timeval start, end;
int file_size;
int con_index = -1;
int sender = 0;

//Declaring external variables
extern int server_socket;
extern int num_conn;
extern char *port_str;
extern char *server_or_client;
extern IPandPort ConnectedPeers[3];
extern char host_name[50];

void uploadFile();
void displayStatisticsForLastUpload();


#endif /* FILEUPLOADER_H_ */

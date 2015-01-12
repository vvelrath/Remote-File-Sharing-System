/**
 * File Name: FileDownloader.c
 * @vvelrath_assignment1
 * @author  Vivekanandh Vel Rathinam <vvelrath@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */

#include "../include/FileDownloader.h"

#define FILE_NAME_LENGTH 150

typedef struct{
	int  socket_id;
	int  connection_id;
	char host_name[100];
	char IP[INET_ADDRSTRLEN];
	char Port[10];
}IPandPort;

extern char* server_or_client;
extern int server_socket;
extern int num_conn;
extern IPandPort ConnectedPeers[3];

//Local Declarations
int args_given = 0;

void downloadFile(int connectionID, char file_name[]);

void downloadFiles(){

	char *connectionID_str;
	int connectionID = 0;
	args_given = 0;

	//Initial validations
	if(strcmp(server_or_client,"s") == 0){
		printf("You are a server. You are not allowed to download files\n");
	}
	else if(server_socket == -1){
		printf("Register with a server and perform other operations\n");
	}
	else{
		while(1){
			//Getting the Connection ID
			connectionID_str = strtok(NULL, " ");
			if(connectionID_str == NULL) break;
			connectionID = atoi(connectionID_str);

			//Getting the file name to download
			char *file_name_ptr = strtok(NULL," ");

			//Setting the number of arguments
			args_given += 1;

			downloadFile(connectionID, file_name_ptr);
		}

		//Validation if no arguments are given
		if(args_given == 0){
			printf("Usage: DOWNLOAD <connection id 1> <file 1> <connection id 2> <file 2> <connection id 3> <file 3>\n");
		}
	}
}


void downloadFile(int connectionID, char* file_name_ptr){

	int isConnected = 0;
	char download_message[100];
	int remote_socket = -1;
	int numbytes = 0;
	char valid_or_invalid[100];
	int y = 0;
	char file_name[FILE_NAME_LENGTH];

	//Checking if the connection ID is valid
	for(y = 0;y < num_conn; y++){
		if(ConnectedPeers[y].connection_id == connectionID){
			isConnected = 1;
			remote_socket = ConnectedPeers[y].socket_id;
			break;
		}
	}

	if(isConnected == 0){
		printf("Connection ID %d is invalid\n",connectionID);
	}else if(file_name_ptr == NULL){
		printf("No file name provided for Connection ID %d\n",connectionID);
	}
	else{
		strcpy(download_message,"DOWNLOAD");
		strcpy(file_name, file_name_ptr);

		if(send(remote_socket, download_message, sizeof(download_message), 0) == -1){
			perror("Failed to send the notification of download\n");
			exit(1);
		}

		if(send(remote_socket, file_name, sizeof(file_name), 0) == -1){
			perror("Failed to send meta information about the file\n");
			exit(1);
		}

		//Receive the valid or invalid file flag from your peer
		if ((numbytes = recv(remote_socket, valid_or_invalid, sizeof(valid_or_invalid), 0)) == -1) {
			perror("Error in receiving the file flag from the client");
			exit(1);
		}
		valid_or_invalid[numbytes] = '\0';

		if(strcmp(valid_or_invalid,"INVALIDFILE") == 0){
			printf("File name: %s not present in %s\n",file_name, ConnectedPeers[y].host_name);
		}else{
			printf("Downloading File: %s from %s\n",file_name, ConnectedPeers[y].host_name);
		}
	}
}

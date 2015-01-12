/**
 * File Name: FileUploader.c
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
#include "../include/FileUploader.h"
#include "../include/UtilityFunctions.h"

void uploadFile(int remote_socket, int initiator){

	char *connectionID_str;
	int connectionID = 0;
	char *file_path_ptr;
	char file_path[150];
	char *file_name;
	int isConnected = 0;
	con_index = -1;
	int y = 0;
	file_size = 0;
	char buffer[PACKET_SIZE];
	char upload_message[100];
	sender = 1;
	int read_fd = -1;
	int usage_error = 0;

	//Getting the Connection ID and file name to upload only if I am initiating it
	if(initiator == 1){
		connectionID_str = strtok(NULL," ");
		file_path_ptr = strtok(NULL," ");

		if(connectionID_str == NULL || file_path_ptr == NULL){
			usage_error = 1;
		}else{
			connectionID = atoi(connectionID_str);
			strcpy(file_path, file_path_ptr);

			//Initializing the file descriptor
			read_fd = open(file_path, O_RDONLY, S_IREAD | S_IWRITE);
		}
	}else{
		int numbytes = 0;
		//Receive the file name from the client
		if ((numbytes = recv(remote_socket, file_path, sizeof(file_path), 0)) == -1) {
			perror("Error in receiving the file flag from the client");
			exit(1);
		}
		file_path[numbytes] = '\0';

		//Initializing the file descriptor
		read_fd = open(file_path, O_RDONLY, S_IREAD | S_IWRITE);
	}


	//Checking if the connection ID is valid
	for(y = 0;y < num_conn; y++){
		if(ConnectedPeers[y].connection_id == connectionID ||
		   ConnectedPeers[y].socket_id == remote_socket){
			isConnected = 1;
			con_index = y;
			remote_socket = ConnectedPeers[y].socket_id;
			break;
		}
	}

	//Initial validations
	if(strcmp(server_or_client,"s") == 0){
		printf("You are a server. You are not allowed to upload a file\n");
	}
	else if(server_socket == -1){
		printf("Register with a server and perform other operations\n");
	}
	else if(usage_error == 1){
		printf("Usage: UPLOAD <Connection ID> <File name>\n");
	}
	else if(connectionID == 1){
		printf("You cannot upload a file to a server\n");
	}
	else if(isConnected == 0){
		printf("\nConnection ID %d is invalid",connectionID);
	}
	else if(read_fd == -1){//If file pointer is null, then print an error message
		if(initiator == 1)
			printf("File %s does not exist",file_path);
		else{ // Sending invalid file flag to the downloading process
			strcpy(upload_message,"INVALIDFILE");
			if(send(remote_socket, upload_message, sizeof(upload_message), 0) == -1){
				perror("Failed to send meta information about the file\n");
				exit(1);
			}
		}
	}
	else{// Every thing is fine , just upload

		// Sending valid file flag to the downloading process
		if(initiator == 0){
			strcpy(upload_message,"VALIDFILE");
			if(send(remote_socket, upload_message, sizeof(upload_message), 0) == -1){
				perror("Failed to send meta information about the file\n");
				exit(1);
			}
		}

		//Sending this STARTUPLOAD header to the remote machine to make it understand
		//that a file is ready to be downloaded
		strcpy(upload_message,"STARTUPLOAD");
		if(send(remote_socket, upload_message, sizeof(upload_message), 0) == -1){
			perror("Failed to send meta information about the file\n");
			exit(1);
		}

		printf("Uploading File: %s to %s\n",file_path,ConnectedPeers[y].host_name);

		//Sending the file name to the remote machine
		//Getting the last part of the file name
		char filename_prev[100];

		strcpy(filename_prev, file_path);
		file_name = strtok(file_path, "/");
		while(file_name != NULL) {
			strcpy(filename_prev, file_name);
			file_name = strtok(NULL, "/");
		}

		strcpy(upload_message,filename_prev);
		if(send(remote_socket, upload_message, sizeof(upload_message), 0) == -1){
			perror("Failed to send meta information about the file\n");
			exit(1);
		}

		//Getting the start time of uploading the file
		gettimeofday(&start, NULL);

		//Uploading the file to the remote destination
		int buffer_length = 0;
		while ((buffer_length = read(read_fd, buffer, sizeof(buffer))) > 0)
		{
			file_size = file_size + buffer_length;
			buffer[buffer_length] = '\0';
			send(remote_socket, buffer, sizeof(buffer), 0);
			strcpy(buffer,"");
		}

		//Closing the file after the upload
		close(read_fd);

		//Getting the end time of uploading the file
		gettimeofday(&end, NULL);

		//Sending this ENDUPLOAD header to the remote machine to make it understand
		//that all the file data has been received
		strcpy(buffer,"ENDUPLOAD");
		if(send(remote_socket, buffer, sizeof(buffer), 0) == -1){
			perror("Failed to send the ending message");
			exit(1);
		}

		//Sending the size of the file
		strcpy(buffer,"");
		sprintf(buffer, "%d", file_size);

		if(send(remote_socket, buffer, sizeof(buffer), 0) == -1){
			perror("Failed to send the file size");
			exit(1);
		}

		printf("\nUpload Completed\n");

		//Upload statistics
		displayStatisticsForLastUpload();
	} // end file upload else
}


//Display the statistics for last upload
void displayStatisticsForLastUpload(){

	//Measuring the time taken for the upload
	long double time_taken = ((1000000 * end.tv_sec + end.tv_usec) - (1000000 * start.tv_sec + start.tv_usec));

	//Transmission rate
	long double trans_rate = file_size * 8/time_taken * 1000000;

	if(sender == 1)
		printf("\nTx: %s -> %s, File Size: %d bytes, Time Taken: %.10Lf seconds, Tx Rate: %Lf bits/second\n",
				host_name,ConnectedPeers[con_index].host_name, file_size, time_taken/1000000, trans_rate);
	else
		printf("\nRx: %s -> %s, File Size: %d bytes, Time Taken: %.10Lf seconds, Rx Rate: %Lf bits/second\n\n\n",
				ConnectedPeers[con_index].host_name, host_name, file_size, time_taken/1000000, trans_rate);

	//Update Statistics
	updateStatistics(sender,host_name,port_str,ConnectedPeers[con_index].host_name,ConnectedPeers[con_index].Port, file_size, time_taken);

	//Send the statistics to server
	sendStatisticsToServer(sender,host_name,port_str,ConnectedPeers[con_index].host_name,ConnectedPeers[con_index].Port, file_size, time_taken);
}


//Receive the uploaded file from your peer
void receiveUploadedFile(int i){

	char peer_buffer[PACKET_SIZE];
	char file_name[100];
	int numbytes = 0;
	int y = 0;
	int index = -1;
	file_size = 0;
	sender = 0;
	char *file_ext;

	//Checking if the connection ID is valid
	for(y = 0;y < num_conn; y++){
		if(ConnectedPeers[y].socket_id == i){
			con_index = y;
			break;
		}
	}

	//Receive the file name from your peer
	if ((numbytes = recv(i, file_name, sizeof(file_name), 0)) == -1) {
		perror("Error in receiving the file name from the client");
		exit(1);
	}
	file_name[numbytes] = '\0';

	//Printing the detials of the remote machine's upload
	for(y = 0;y < num_conn; y++){
		if(ConnectedPeers[y].socket_id == i){
			index = y;
		}
	}
	printf("Peer: %s uploading %s to your machine\n",ConnectedPeers[index].host_name, file_name);

	//Creating or appending a file in your current directory
	int fd = open(file_name, O_RDWR|O_CREAT|O_TRUNC,0777); // Code adapted from http://stackoverflow.com/questions/2395465/create-a-file-in-linux-using-c

	//Getting the file extension
	strtok(file_name, ".");
	file_ext = strtok(NULL, ".");

	//Getting the start time of the upload
	gettimeofday(&start, NULL);

	//Download the file and place it in the same directory
	while(1){
		//Clearing the buffer
		strcpy(peer_buffer,"");

		//Receive the contents from the socket to the buffer
		if ((numbytes = recv(i, peer_buffer, sizeof(peer_buffer), 0)) == -1) {
			perror("Error in receiving the contents from the socket");
			exit(1);
		}
		peer_buffer[numbytes] = '\0';

		//Determining the size of the buffer for text files -- weird but works
		if(strcmp(file_ext,"txt") == 0){
			numbytes = strlen(peer_buffer);
		}

		//If the ENDUPLOAD message is received, break from the loop
		if(strcmp(peer_buffer,"ENDUPLOAD") == 0){
			//Receive the file name from your peer
			strcpy(peer_buffer,"");
			if ((numbytes = recv(i, peer_buffer, sizeof(peer_buffer), 0)) == -1) {
				perror("Error in receiving the file name from the client");
				exit(1);
			}
			peer_buffer[numbytes] = '\0';
			file_size = atoi(peer_buffer);
			break;
		}

		//Write the buffer contents to the file
		write(fd, peer_buffer, numbytes); // numbytes for image and strlen for text
	}

	//Closing the file descriptor
	close(fd);

	//Getting the start time of the upload
	gettimeofday(&end, NULL);

	printf("\nThe remote machine's upload to your file system has been completed\n");

	//Upload statistics
	displayStatisticsForLastUpload();
}

/**
 * File Name: Statistics.c
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

#include <../include/Statistics.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PEERS 30
extern int server_socket;
extern char* server_or_client;

//User defined data type for entering statistic information
typedef struct{
	//Memory info
	int memory_used;

	//Sender and Receiver info
	char host_name1[100];
	char port1[10];
	char host_name2[100];
	char port2[10];

	//Upload info
	int num_uploads;
	int uploaded_bytes;
	long double upload_time_in_us; // Micro Seconds

	//Download info
	int num_downloads;
	int downloaded_bytes;
	long double download_time_in_us; // Micro Seconds
}Stats;

//This variable will hold all the statistics
Stats statistics[MAX_PEERS];// Maximum information of 10 peers



//Updating the statistics in Server
void updateStatisticsInServer(int client_socket){
	char stat_info[200];
	int numbytes = 0;
	int sender;


	//Receive the stat info from your client
	if ((numbytes = recv(client_socket, stat_info, sizeof(stat_info), 0)) == -1) {
		perror("Error in receiving the file name from the client");
		exit(1);
	}
	stat_info[numbytes] = '\0';

	//Getting the Host's information
	sender = atoi(strtok(stat_info,";"));
	char *host_name1 = strtok(NULL,";");
	char *port1 = strtok(NULL,";");
	char *host_name2 = strtok(NULL,";");
	char *port2 = strtok(NULL,";");

	int file_size = atoi(strtok(NULL,";"));
	long double time_taken = atoi(strtok(NULL,";"));

	//Updating the statistics
	updateStatistics(sender, host_name1, port1, host_name2, port2, file_size, time_taken);
}


//Updating the statistics of file uplaods and downloads
void updateStatistics(int sender, char my_host_name[], char my_port_no[], char remote_host_name[], char remote_port_no[], int file_size, long double time_taken){

	int is_info_found = 0;
	int use_index = -1;
	int l;

	//Return an index in the statistics array if info is present
	for(l = 0; l < MAX_PEERS; l++){
		if(strcmp(statistics[l].host_name1,my_host_name) == 0 &&
		   strcmp(statistics[l].port1,my_port_no) == 0 &&
		   strcmp(statistics[l].host_name2,remote_host_name) == 0 &&
		   strcmp(statistics[l].port2,remote_port_no) == 0){
			is_info_found = 1;
			use_index = l;
			break;
		}
	}

	//If there is no information found in the statistics array, find a minimum index where there is no info
	if(is_info_found == 0){
		//Looping through the array to find an empty space
		for(l = 0; l < MAX_PEERS; l++){
			if(statistics[l].memory_used == -1){
				use_index = l;
				break;
			}
		}
	}

	//Copying the sender and receiver information
	strcpy(statistics[use_index].host_name1, my_host_name);
	strcpy(statistics[use_index].port1, my_port_no);

	strcpy(statistics[use_index].host_name2, remote_host_name);
	strcpy(statistics[use_index].port2, remote_port_no);

	//Update the memory usage information
	statistics[use_index].memory_used = 1;

	if(sender == 1){ // You have uploaded something
		statistics[use_index].num_uploads = statistics[use_index].num_uploads + 1;
		statistics[use_index].uploaded_bytes = statistics[use_index].uploaded_bytes + file_size;
		statistics[use_index].upload_time_in_us = statistics[use_index].upload_time_in_us + time_taken;
	}
	else{ // You have downloaded something
		statistics[use_index].num_downloads = statistics[use_index].num_downloads + 1;
		statistics[use_index].downloaded_bytes = statistics[use_index].downloaded_bytes + file_size;
		statistics[use_index].download_time_in_us = statistics[use_index].download_time_in_us + time_taken;
	}
}


//Sending the statistics to server
void sendStatisticsToServer(int sender, char my_host_name[], char my_port_no[], char remote_host_name[], char remote_port_no[], int file_size, long double time_taken){
	char update_stat[100];
	char stat_info[200];
	char file_size_str[50];
	char time_taken_str[50];

	strcpy(update_stat,"UPDATESTAT");

	//Sending a update stat notification
	if(send(server_socket, update_stat, sizeof(update_stat), 0) == -1){
		perror("Failed to send the statistics notification");
	}

	//Concatenating the host information
	sprintf(stat_info, "%d", sender); 	strcat(stat_info,";");
	strcat(stat_info, my_host_name);	strcat(stat_info,";");
	strcat(stat_info, my_port_no);		strcat(stat_info,";");
	strcat(stat_info, remote_host_name);strcat(stat_info,";");
	strcat(stat_info, remote_port_no);	strcat(stat_info,";");

	//Concatenating the file size
	sprintf(file_size_str, "%d", file_size);
	strcat(stat_info, file_size_str);	strcat(stat_info,";");

	//Concatenating the file size
	sprintf(time_taken_str, "%Lf", time_taken);
	strcat(stat_info, time_taken_str);

	//Sending the statistic info
	if(send(server_socket, stat_info, sizeof(stat_info), 0) == -1){
		perror("Failed to send the statistics information");
	}
}


//Initializing the statistics information
void initializeStats(){
	int l;
	for(l = 0; l < MAX_PEERS; l++){
		//Initializing the memory slots to -1
		statistics[l].memory_used = -1;

		//Initializing the host and port names
		strcpy(statistics[l].host_name1, "");
		strcpy(statistics[l].port1, "");
		strcpy(statistics[l].host_name2, "");
		strcpy(statistics[l].port2, "");

		//Initializing the number of uploads and downloads
		statistics[l].num_uploads = 0;
		statistics[l].num_downloads = 0;

		//Initializing the number of bytes uploaded and downloaded
		statistics[l].uploaded_bytes = 0;
		statistics[l].downloaded_bytes = 0;

		//Initializing the total time taken for upload and download
		statistics[l].upload_time_in_us = 1; // To avoid divide by zero
		statistics[l].download_time_in_us = 1; // To avoid divide by zero
	}
}

//Display statistics
void displayStatistics(){
	if(strcmp(server_or_client,"s") == 0)
		displayStatisticsInServer();
	else
		displayStatisticsInClient();
}


//Display statistics in client
void displayStatisticsInClient(){
	int y = 0;
	long double upload_rate;
	long double download_rate;

	printf("----------------------------------Statistics----------------------------------\n");
	printf("HOSTNAME                           PORT    TOTAL UPLOADS   AVERAGE UPLOAD SPEED(bps)   TOTAL DOWNLOADS   AVERAGE DOWNLOAD SPEED(bps)\n\n");
	for(y = 0;y < MAX_PEERS; y++){
		if(statistics[y].memory_used == 1){
			//Upload speed
			upload_rate = statistics[y].uploaded_bytes * 8/statistics[y].upload_time_in_us * 1000000;

			//Download speed
			download_rate = statistics[y].downloaded_bytes * 8/statistics[y].download_time_in_us * 1000000;

			printf("%-35s%-8s%-16d%-16Lf             %-18d%-8Lf\n\n",statistics[y].host_name2,statistics[y].port2,
										  statistics[y].num_uploads, upload_rate,
										  statistics[y].num_downloads, download_rate);
		}
	}
}


//Display statistics in server
void displayStatisticsInServer(){
	int y = 0;
	long double upload_rate;
	long double download_rate;

	printf("----------------------------------Statistics----------------------------------\n");
	printf("HOSTNAME1                           HOSTNAME2                           TOTAL UPLOADS   AVERAGE UPLOAD SPEED(bps)   TOTAL DOWNLOADS   AVERAGE DOWNLOAD SPEED(bps)\n\n");
	for(y = 0;y < MAX_PEERS; y++){
		if(statistics[y].memory_used == 1){
			//Upload speed
			upload_rate = statistics[y].uploaded_bytes * 8/statistics[y].upload_time_in_us * 1000000;

			//Download speed
			download_rate = statistics[y].downloaded_bytes * 8/statistics[y].download_time_in_us * 1000000;

			printf("%-35s%-35s%-16d%-16Lf             %-18d%-8Lf\n\n",statistics[y].host_name1,statistics[y].host_name2,
										  statistics[y].num_uploads, upload_rate,
										  statistics[y].num_downloads, download_rate);
		}
	}
}

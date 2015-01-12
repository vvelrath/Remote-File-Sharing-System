/**
 * File Name: ServerUtility.c
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
#include "../include/ServerUtility.h"

//Defining a  user defined structure
typedef struct{
	int  socket_id;
	int  connection_id;
	char host_name[100];
	char IP[INET_ADDRSTRLEN];
	char Port[10];
}IPandPort;


//An external variable
extern IPandPort IPList[4];

void sendConnectionInfoToAllClients(char host_name[], char client_ip[], char client_port[]){

	//Declaring local variables
	int x = 0, y = 0;
	int numbytes = 0;
	struct sockaddr_storage remote_address;
	struct sockaddr_storage addr;

	//Send the server's connection info to the new client as well as the other clients
	for(x = 1;x <= max_socket;x++){
		//Checking if the sockets are set in the master collection
		if(FD_ISSET(x,&master_sockets) && x != listener){
			//Sending the info to the new client
			if(x == client_socket){
				//Loop through all the peers info from the global list
				for(y = 0;y < num_peers; y++){
					char client_info[150];

					if(y == num_peers - 1)
						strcpy(client_info,"ADDLASTPEER ");
					else
						strcpy(client_info,"ADDPEER ");

					strcat(client_info, IPList[y].host_name);
					strcat(client_info, " ");
					strcat(client_info, IPList[y].IP);
					strcat(client_info, " ");
					strcat(client_info, IPList[y].Port);

					if(send(x,client_info,sizeof(client_info),0) == -1){
						perror("Error in Sending data to the new client");
						exit(1);
					}
				}
			}
			//Sending the info to the other connections
			else{
				char client_info[150];
				strcpy(client_info,"ADDLASTPEER ");
				strcat(client_info, host_name);
				strcat(client_info, " ");
				strcat(client_info, client_ip);
				strcat(client_info, " ");
				strcat(client_info, client_port);

				if(send(x,client_info,sizeof(client_info),0) == -1){
					perror("Error in Sending data to the other clients");
					exit(1);
				}
			}
		}
	}

	//Updating the number of peers
	num_peers = num_peers + 1;
}

void removeClientFromNetwork(int socket_id){
	//Declaring local variables
	int x = 0, y = 0;
	int numbytes = 0;
	int index = -1;

	for(y = 0;y < num_peers; y++){
		if(IPList[y].socket_id == socket_id){
			index = y;
		}
	}

	if(FD_ISSET(socket_id,&master_sockets)){
		//Loop through all the peers info from the global list
		FD_CLR(socket_id,&master_sockets);
	}

	//Send the server's connection info to the new client as well as the other clients
	for(x = 1;x <= max_socket;x++){
		//Checking if the sockets are set in the master collection
		if(FD_ISSET(x,&master_sockets) && x != listener){
			//Sending the info to the new client
			char client_info[150];
			strcpy(client_info,"REMOVEPEER ");
			strcat(client_info, IPList[index].host_name);
			strcat(client_info, " ");
			strcat(client_info, IPList[index].IP);
			strcat(client_info, " ");
			strcat(client_info, IPList[index].Port);

			if(send(x,client_info,sizeof(client_info),0) == -1){
				perror("Error in Sending data to the other clients");
				exit(1);
			}
		}
	}

	//Deleting the connection from the Connected Peers List
	for(y = index + 1;y < num_peers; y++){
		IPList[y - 1] = IPList[y];
		IPList[y - 1].connection_id = IPList[y - 1].connection_id - 1;
	}

	//Updating the number of peers
	num_peers = num_peers - 1;
}

//This function lists the server's connections
void listServerConnections(){
	int y = 0;
	printf("---------------------------Server IP List---------------------------\n");
	for(y = 0;y < num_peers; y++){
		printf("%-5d%-35s%-20s%-8d\n",IPList[y].connection_id,IPList[y].host_name,IPList[y].IP,atoi(IPList[y].Port));
	}
}

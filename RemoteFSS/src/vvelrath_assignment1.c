/**
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

#include "../include/global.h"
#include "../include/UtilityFunctions.h"
#include "../include/Statistics.h"

//Declarations
int a = 1;
int i = 0;
int j = 0;
int k = 0;
const void *optval = &a;
struct sockaddr *my_address;
char *command;
char user_input[200];
char *server_or_client = NULL;
struct sockaddr_storage remote_address;
socklen_t remote_addr_length = sizeof(remote_address);
char *port_str;
int exit_flag = 1;

extern int max_socket;
extern int server_socket;
extern int listener;
extern int port_no;
extern char *port_str;
extern int num_peers;
extern int num_conn;
extern fd_set master_sockets, watch_sockets;
extern IPandPort IPList[4];
extern IPandPort ConnectedPeers[3];
extern char host_name[50];

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char *argv[])
{
	//Setting server or client
	server_or_client = argv[1];

	//Setting the listening port number
	port_no = atoi(argv[2]);
	port_str = argv[2];

	//Get my host name
	gethostname(host_name, sizeof(host_name));

	//Compute my Public IP
	computeMyIP();

	//Creating a listening socket
	listener = createListener();

	//Initializing the Master FDS with the listening and the standard input socket numbers
	initializeFDS();

	//Initializing statistics
	initializeStats();

	//Infinite FOR loop to continuously satisfy the network
	for(;;){

		//Copying the master list to the watch list
		memcpy(&watch_sockets,&master_sockets,sizeof(&master_sockets));

		if(select(max_socket+1,&watch_sockets,NULL,NULL,NULL) == -1){
			perror("Error in Select");
			exit(1);
		}

		//Looking for user commands from the input prompt
		if(FD_ISSET(0, &watch_sockets)){

				//Getting the user input
				fgets(user_input, sizeof(user_input), stdin);
				user_input[strlen(user_input) - 1] = '\0';

				//Getting the primary command from user input
				if(strlen(user_input) == 0)
					command = "";
				else
					command = strtok(user_input," ");

			 	if(strcmp(command,"CREATOR") == 0){
					showCreatorInformation();
				} else if(strcmp(command,"HELP") == 0){
					showHelperInformation();
				} else if(strcmp(command,"MYPORT") == 0){
					showListeningPort();
				} else if(strcmp(command,"MYIP") == 0){
					computeMyIP();
				} else if(strcmp(command,"REGISTER") == 0){
					connectToServer();
				} else if(strcmp(command,"CONNECT") == 0){
					connectToPeer();
				} else if(strcmp(command,"LIST") == 0){
					listMyConnections();
				} else if(strcmp(command,"TERMINATE") == 0){
					terminateConnection(0);
				} else if(strcmp(command,"EXIT") == 0){
					exitClient(); if(exit_flag == 1) break;
				} else if(strcmp(command,"UPLOAD") == 0){
					uploadFile(-1,1);
				} else if(strcmp(command,"DOWNLOAD") == 0){
					downloadFiles();
				} else if(strcmp(command,"STATISTICS") == 0){
					displayStatistics();
				} else{
					printf("%s: Command not found",command);
				}
		}

		//Looking for data to read from the existing connections
		for(i = 1;i <= max_socket;i++){
			//Check if the socket is in the watch list
			if(FD_ISSET(i, &watch_sockets)){
				// A connection request from a remote machine
				if(i == listener){
					//Accepting a new connection using the update method
					if((client_socket = accept(listener, (struct sockaddr *)&remote_address,&remote_addr_length)) == -1){
						perror("Error in accept");
						exit(1);
					}

					//Add the new socket to the master list
					FD_SET(client_socket, &master_sockets);

					//Setting the maximum socket number to the last connected client
					if(client_socket > max_socket){
						max_socket = client_socket;
					}

				    //Send the new connection's information to all existing clients if I am a server
					if(strcmp(server_or_client,"s") == 0){ // if you are a server

						char client_port[10];
						char host_name[100];
						char client_ip[INET_ADDRSTRLEN];

						int numbytes = 0;
						struct sockaddr_storage remote_address;
						struct sockaddr_storage addr;

						//Oh Great!! Now lets get the client's listening port number that he has sent
						if ((numbytes = recv(client_socket, client_port, sizeof(client_port), 0)) == -1) {
							perror("Error in receiving the client's port");
							exit(1);
						}
						client_port[numbytes] = '\0';

						//I also want the client's IP
						getpeername(client_socket, (struct sockaddr*)&addr, &remote_addr_length);
					    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
					    inet_ntop(AF_INET, &s->sin_addr, client_ip, sizeof client_ip);

					    //I want the client's host name
					    struct sockaddr* sa = (struct sockaddr*)&addr;
					    getnameinfo(sa, remote_addr_length, host_name, sizeof(host_name), NULL, 0, 0);

					    //Using this function to send the information to all clients
						sendConnectionInfoToAllClients(host_name, client_ip, client_port);

						//Fill the IPandPort structure with the new client's info
					    //Adding the new IP and port to the global list of peers
						strcpy(IPList[num_peers - 1].IP, client_ip);
						strcpy(IPList[num_peers - 1].Port, client_port);
						strcpy(IPList[num_peers - 1].host_name, host_name);
						IPList[num_peers - 1].connection_id = num_peers;
						IPList[num_peers - 1].socket_id = client_socket;

					}//endif--server
					else{ // if I am a client
						//Update the number of connected peers
						num_conn = num_conn + 1;

						char client_port[10];
						int numbytes = 0;
						char client_ip[INET_ADDRSTRLEN];
						char host_name[100];
						struct sockaddr_storage remote_address;
						struct sockaddr_storage addr;

						//I also want the peer's IP and port
						if ((numbytes = recv(client_socket, client_port, sizeof(client_port), 0)) == -1) {
						     perror("Error in receiving the client's port");
						     exit(1);
						}
						client_port[numbytes] = '\0';

						//I also want the client's IP
						getpeername(client_socket, (struct sockaddr*)&addr, &remote_addr_length);
					    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
					    inet_ntop(AF_INET, &s->sin_addr, client_ip, sizeof client_ip);

					    //I want the client's host name
					    struct sockaddr* sa = (struct sockaddr*)&addr;
					    getnameinfo(sa, remote_addr_length, host_name, sizeof(host_name), NULL, 0, 0);

					    //Adding the new IP and port to the global list of peers
						strcpy(ConnectedPeers[num_conn - 1].IP, client_ip);
						strcpy(ConnectedPeers[num_conn - 1].Port, client_port);
						strcpy(ConnectedPeers[num_conn - 1].host_name, host_name);
						ConnectedPeers[num_conn - 1].socket_id = client_socket;
						ConnectedPeers[num_conn - 1].connection_id = num_conn + 1;
					}
				}
				//If I receive data from the server
				else if(i == server_socket){
					char server_buffer[150];
					int numbytes = 0;
					int m = 0;
					int position = -1;

					if ((numbytes = recv(i, server_buffer, sizeof(server_buffer), 0)) == -1) {
						perror("Error in receiving data");
					    exit(1);
					}

					//NULL Terminating the string
					server_buffer[numbytes] = '\0';

					char *command = strtok(server_buffer," ");
					char *HostName = strtok(NULL," ");
					char *IP = strtok(NULL," ");
					char *Port = strtok(NULL," ");


					if(strcmp(command,"ADDPEER") == 0 || strcmp(command,"ADDLASTPEER") == 0){
						//Updating the number of peers
						num_peers = num_peers + 1;

						//Fill the IPandPort structure with the new client's info and it to the global list
						strcpy(IPList[num_peers - 1].IP, IP);
						strcpy(IPList[num_peers - 1].Port, Port);
						strcpy(IPList[num_peers - 1].host_name, HostName);
						IPList[num_peers - 1].connection_id = num_peers;
					}
					else
					{
						for(m = 0;m < num_peers; m++){
							if(strcmp(IPList[m].IP,IP) == 0 &&
							   strcmp(IPList[m].host_name,HostName) == 0 &&
							   strcmp(IPList[m].Port,Port) == 0){
								position = m;
								break;
							}
						}

						for(m = position+1;m < num_peers; m++){
							IPList[m - 1] = IPList[m];
							IPList[m - 1].connection_id = IPList[m - 1].connection_id - 1;
						}

						//Updating the number of peers
						num_peers = num_peers - 1;

					}

					//Ok!! The list has been updated. Now lets display the current peers in the network
					if(strcmp(command,"ADDLASTPEER") == 0 || strcmp(command,"REMOVEPEER") == 0){
						listServerConnections();
					}
				}
				// Existing connections have data for me
				else{
					char peer_buffer[100];
					char file_name[100];
					int numbytes = 0;
					int y = 0;
					int index = -1;

					//Receive the data from your peer
					if ((numbytes = recv(i, peer_buffer, sizeof(peer_buffer), 0)) == -1) {
						perror("Error in receiving data from the client");
						exit(1);
					}
					peer_buffer[numbytes] = '\0';

					//If no data is received, then the remote machine has closed the connection
					if(numbytes == 0){
						if(strcmp(server_or_client,"s") == 0)
							removeClientFromNetwork(i);
						else
							closeTerminatedConnection(i);
					}
					else if(strcmp(peer_buffer,"STARTUPLOAD") == 0){// Your peer is sending you a file
						receiveUploadedFile(i);
					}
					else if(strcmp(peer_buffer,"DOWNLOAD") == 0){// Your peer is requesting you a file
						uploadFile(i, 0);
					}
					else if(strcmp(peer_buffer,"UPDATESTAT") == 0){// Some client is about to send stats
						updateStatisticsInServer(i);
					}
				}
			}
		}

		//Input Prompt after satisfying all the requests in the watch list
		printf("\n>> ");
		fflush(stdout);
	}

	printf("\nYour Process is terminated\n\n");

	return 0;
}


//This function helps us to create a socket and listen on a specific port
int createListener(){
	//Local listening socket number
	int listen_socket = 0;
	struct addrinfo *addr_info;

	//Creating a listener socket
	if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Error creating a Socket");
		exit(1);
	}

	//Gets rid of the Address Already in USE message
	if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
		perror("Error setting options on the socket");
		exit(1);
	}

	//Auto filling the Sockaddr data structure
	addr_info = getAddrInfoStructure(NULL,port_str);

	for(p = addr_info; p != NULL; p = p->ai_next) {
		if(bind(listen_socket,p->ai_addr,p->ai_addrlen) < 0)
			continue;
		else
			break;
	}

	//Starting to listen on the bound socket
	if(listen(listen_socket,5) == -1){
		close(listen_socket);
		perror("Error listening in this socket");
		exit(1);
	}
	return listen_socket;
}


//This function initializes the master and file descriptor sets
void initializeFDS(){
	//Initializing the master and the watch lists
	FD_ZERO(&master_sockets);
	FD_ZERO(&watch_sockets);

	//Adding the Standard Input File descriptor to the master connectors
	FD_SET(0, &master_sockets);

	//Adding the listening socket to the master connectors
	FD_SET(listener, &master_sockets);

	//Setting the maximum socket number with the last created listening socket number
	max_socket = listener;

	//To make sure that the data is flushed to standard output even without a new line
	setbuf(stdout, NULL);
	printf(">> ");
}


//This functions lists my connections
void listMyConnections(){
	if(strcmp(server_or_client,"s") == 0)
		listServerConnections();
	else
		listClientConnections();
}

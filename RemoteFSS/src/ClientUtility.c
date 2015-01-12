/**
 * File Name: ClientUtility.c
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

#include "../include/ClientUtility.h"
#include "../include/UtilityFunctions.h"

typedef struct{
	int  socket_id;
	int  connection_id;
	char host_name[100];
	char IP[INET_ADDRSTRLEN];
	char Port[10];
}IPandPort;


extern char ip_str[INET_ADDRSTRLEN];
extern char *server_or_client;
extern IPandPort IPList[4];
extern IPandPort ConnectedPeers[3];
extern int exit_flag;

char serverIP[INET_ADDRSTRLEN];
char serverPort[10];

int y = 0;
int x = 0;

void connectToServer(char input_args[]){
	struct addrinfo *addr_info;
	struct addrinfo *p;
	int connect_flag = 0;
	char *serverIP_ptr;
	char *serverPort_ptr;

	//Getting the IP and the port of the server
	serverIP_ptr = strtok(NULL," ");
	serverPort_ptr = strtok(NULL," ");

	//Initial validations
	if(strcmp(server_or_client,"s") == 0){
		printf("You are a server. You are not allowed to register with any one else\n");
	}
	else if(serverIP_ptr == NULL || serverPort_ptr == NULL){
		printf("Usage: REGISTER <SERVER IP> <PORT NO>\n");
	}
	else if(strcmp(serverIP_ptr,ip_str) == 0 && strcmp(serverPort_ptr,port_str) == 0){
		printf("Self registrations are not allowed\n");
	}
	else if(server_socket != -1){
		printf("Already connected with a server. You can try connecting with peers in the network\n");
	}
	else{

		strcpy(serverIP, serverIP_ptr);
		strcpy(serverPort, serverPort_ptr);

		//Auto filling the Sockaddr data structure
		addr_info = getAddrInfoStructure(serverIP,serverPort);

		//Code adapted from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#simpleclient starts here
		for(p = addr_info; p != NULL; p = p->ai_next) {

			if((server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
				continue;

			if(connect(server_socket,p->ai_addr,p->ai_addrlen) < 0)
				continue;
			else{
				connect_flag = 1;
				break;
			}
		}
		//Code adapted from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#simpleclient ends here

		if(connect_flag == 0){
			perror("Error connecting to server");
			close(server_socket);
			server_socket = -1;
		}
		else{
			//Add the server's socket to the master list
			FD_SET(server_socket, &master_sockets);

			//Setting the maximum socket number to the last connected client
			if(server_socket > max_socket){
				max_socket = server_socket;
			}

			//Send the listening port of your machine to the server
			char port_string[10];
			strcpy(port_string,port_str);
			if(send(server_socket, port_string, sizeof(port_string), 0) == -1){
				perror("Error in Sending Listening Port Number to the server");
				exit(1);
			}
		}
	}//end else
}


//Function: For connecting to a peer in the network

void connectToPeer(char input_args[]){
	int y = 0;
	char *peerIP;
	char *peerPort;
	struct addrinfo *addr_info;
	struct addrinfo *p;
	int connect_flag = 0;
	int connect_valid = 0;
	int already_connected = 0;
	int peer_socket = -1;
	char peer_host_name[100];
	int reg_index = -1;

	//Getting the IP and the port of the peer to connect
	peerIP = strtok(NULL," ");
	peerPort = strtok(NULL," ");


	//Checking if the IP and port to connect is already connected with me
	for(y = 0;y < num_conn; y++){
		if((strcmp(ConnectedPeers[y].IP, peerIP) == 0 || strcmp(ConnectedPeers[y].host_name, peerIP) == 0) &&
			strcmp(ConnectedPeers[y].Port, peerPort) == 0){
			already_connected = 1;
			break;
		}
	}

	//Checking if the IP and port to connect is present in the Server IP List
	for(y = 0;y < num_peers; y++){
		if((strcmp(IPList[y].IP, peerIP) == 0 || strcmp(IPList[y].host_name, peerIP) == 0) &&
			strcmp(IPList[y].Port, peerPort) == 0)
		{
			strcpy(peer_host_name,IPList[y].host_name);
			connect_valid = 1;
			reg_index = y;
			break;
		}
	}


	if(strcmp(server_or_client,"s") == 0){
		printf("You are a server. You are not allowed to connect with any one else\n");
	}
	else if(server_socket == -1){
		printf("Register yourself to a server. Then perform other operations\n");
	}
	else if(peerIP == NULL || peerPort == NULL){
		printf("Usage: CONNECT <destination> <port no>\n");
	}
	else if(strcmp(peerIP,ip_str) == 0 && strcmp(peerPort,port_str) == 0){
		printf("Self connections are not allowed\n");
	}
	else if(already_connected == 1){
		printf("\nConnection already established. Try other operations");
	}
	else if(connect_valid == 0){
		printf("\nPeer not in our network. Not allowed to connect");
	}
	else{ // Everything ok !! Lets start connecting

		//Auto filling the Sockaddr data structure
		addr_info = getAddrInfoStructure(peerIP,peerPort);

		//Code adapted from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#simpleclient starts here
		for(p = addr_info; p != NULL; p = p->ai_next) {

			if((peer_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
				continue;

			if(connect(peer_socket,p->ai_addr,p->ai_addrlen) < 0)
				continue;
			else{
				connect_flag = 1;
				break;
			}
		}
		//Code adapted from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#simpleclient ends here

		if(connect_flag == 0){
			perror("Error connecting to peer");
			close(peer_socket);
			peer_socket = -1;
		}

		//Add the server's socket to the master list
		FD_SET(peer_socket, &master_sockets);

		//Setting the maximum socket number to the last connected client
		if(peer_socket > max_socket){
			max_socket = peer_socket;
		}

		//Update the number of connections
		num_conn = num_conn + 1;

		//Adding the new IP and port to the global list of peers
		strcpy(ConnectedPeers[num_conn - 1].IP, IPList[reg_index].IP);
		strcpy(ConnectedPeers[num_conn - 1].Port, peerPort);
		strcpy(ConnectedPeers[num_conn - 1].host_name, peer_host_name);
		ConnectedPeers[num_conn - 1].socket_id = peer_socket;
		ConnectedPeers[num_conn - 1].connection_id = num_conn + 1;

		//Send the listening port of your machine to the peer
		if(send(peer_socket, port_str, strlen(port_str), 0) == -1){
			perror("Error in Sending Listening Port Number to the peer");
			exit(1);
		}

		printf("You are connected to a peer\n");
	}// end else
}


//This function terminates a connection with a client
void terminateConnection(int connectionToTerminate){

	char *connectionToTerminate_str;
	int isConnected = 0;
	int socket_id = -1;
	int index = -1;
	int usage_error = 0;

	//Getting the connection ID of the connection to terminate
	if(connectionToTerminate == 0){ // Get connection id from standard input
		connectionToTerminate_str = strtok(NULL," ");

		//Input Validation
		if(connectionToTerminate_str == NULL){
			usage_error = 1;
		}else{
			connectionToTerminate = atoi(connectionToTerminate_str);
		}
	}

	//Checking if the IP and port to connect is already connected with me
	for(y = 0;y < num_conn; y++){
		if(ConnectedPeers[y].connection_id == connectionToTerminate){
			isConnected = 1;
			index = y;
			socket_id = ConnectedPeers[y].socket_id;
			break;
		}
	}

	//Performing initial validations
	if(strcmp(server_or_client,"s") == 0){
		printf("You are a server. You are not allowed to terminate connections with any one else\n");
	}
	else if(server_socket == -1){
		printf("Register yourself to a server. Then perform other operations\n");
	}
	else if(usage_error == 1){
		printf("Usage: TERMINATE <Connection ID>\n");
	}
	else if(connectionToTerminate == 1){
		printf("You cannot terminate a connection to the server. Use the EXIT command to come out of the network\n");
	}
	else if(isConnected == 0){
		printf("\nConnection ID %d is invalid",connectionToTerminate);
	}
	else{
		//Clearing the socket id from the master collection
		if(FD_ISSET(socket_id, &master_sockets)){
			FD_CLR(socket_id, &master_sockets);
		}

		//Setting the maximum socket number to the last connected client
		if(socket_id == max_socket){
			max_socket = max_socket - 1;
		}

		printf("\nConnection has been terminated with IP: %s listening on Port: %s\n",ConnectedPeers[index].IP,ConnectedPeers[index].Port);

		//Deleting the connection from the Connected Peers List
		for(y = index + 1;y < num_conn; y++){
			ConnectedPeers[y - 1] = ConnectedPeers[y];
			ConnectedPeers[y - 1].connection_id = ConnectedPeers[y - 1].connection_id - 1;
		}

		//Updating the number of connections
		num_conn = num_conn - 1;

		//Closing the socket connection
		close(socket_id);
	}
}

//Remote machine has already closed has already terminated the connection from his side.
//Now its our turn to close the socket from our side and update the file descriptor set
void closeTerminatedConnection(int socket_id){

	int index = -1;

	//Retrieving the index of the socket from the Connected List
	for(y = 0;y < num_conn; y++){
		if(ConnectedPeers[y].socket_id == socket_id){
			index = y;
			break;
		}
	}

	//Clearing the socket id from the master collection
	if(FD_ISSET(socket_id, &master_sockets)){
		FD_CLR(socket_id, &master_sockets);
	}

	//Setting the maximum socket number to the last connected client
	if(socket_id == max_socket){
		max_socket = max_socket - 1;
	}

	printf("\nIP: %s listening on Port: %s has terminated its connection with you\n",ConnectedPeers[index].IP,ConnectedPeers[index].Port);

	//Deleting the connection from the Connected Peers List
	for(y = index + 1;y < num_conn; y++){
		ConnectedPeers[y - 1] = ConnectedPeers[y];
		ConnectedPeers[y - 1].connection_id = ConnectedPeers[y - 1].connection_id - 1;
	}

	//Updating the number of connections
	num_conn = num_conn - 1;

	//Closing the socket connection
	close(socket_id);
}


//This function is used for exiting the client and closing all the connections
void exitClient(){

	exit_flag = 1;
	//Performing initial validations
	if(strcmp(server_or_client,"s") == 0 && num_peers > 0){
		exit_flag = 0;
		printf("You are not allowed to exit until all the clients have exited\n");
	}else if(strcmp(server_or_client,"s") == 0 && num_peers == 0){
		printf("You dont have any more clients. Just terminating the process\n");
	}else if(server_socket == -1){
		printf("You are not connected with a server. Just terminating the current process\n");
	}else{
		int num_conn_fixed = num_conn;

		//Termainate all the existing connections
		for(x = 0;x < num_conn_fixed; x++){
			terminateConnection(ConnectedPeers[0].connection_id);
		}

		//Clearing the server socket from the master collection
		if(FD_ISSET(server_socket, &master_sockets)){
			FD_CLR(server_socket, &master_sockets);
		}

		close(server_socket);

		printf("You have been removed from the network\n");

	}// end else
}


//This function lists the client's connections
void listClientConnections(){

	if(server_socket == -1)
		printf("Register yourself to a server. Then perform the listing\n");
	else{
		//Compute the server's host name
		struct sockaddr_storage addr;
		char server_host_name[100];
		int server_conn_id = 1;

		getpeername(server_socket, (struct sockaddr*)&addr, &remote_addr_length);
	    struct sockaddr* sa = (struct sockaddr*)&addr;
	    getnameinfo(sa, remote_addr_length, server_host_name, sizeof(server_host_name), NULL, 0, 0);

		printf("--------------My Connections---------------\n");
		printf("%-5d%-35s%-20s%-8d\n",server_conn_id,server_host_name,serverIP,atoi(serverPort));

		for(y = 0;y < num_conn; y++){
			printf("%-5d%-35s%-20s%-8d\n",ConnectedPeers[y].connection_id,ConnectedPeers[y].host_name,ConnectedPeers[y].IP,atoi(ConnectedPeers[y].Port));
		}
	}
}

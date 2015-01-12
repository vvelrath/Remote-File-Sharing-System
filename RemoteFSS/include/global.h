#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define CMDLINEINPSIZE 50

#define HOSTNAME_LEN 128

#define MAX_PARALLEL_DOWNLOAD 3

//Global Declarations
int max_socket;
int server_socket = -1;
int client_socket = 0;
int listener;
int port_no;
int num_peers = 0;
int num_conn = 0;
char *port_str;
fd_set master_sockets, watch_sockets;

typedef struct{
	int  socket_id;
	int  connection_id;
	char host_name[100];
	char IP[INET_ADDRSTRLEN];
	char Port[10];
}IPandPort;

IPandPort IPList[4];
IPandPort ConnectedPeers[3];

//Function declarations for vvelrath_assignment1.c
int createListener();
void initializeFDS();
void listMyConnections();

#endif

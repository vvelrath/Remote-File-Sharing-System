/*
 * ServerUtility.h
 *
 *  Created on: Sep 20, 2014
 *      Author: vivekanandh
 */

#ifndef SERVERUTILITY_H_
#define SERVERUTILITY_H_

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>


//Declaring external variables
extern int client_socket;
extern int max_socket;
extern int num_peers;
extern int listener;
extern int port_no;
extern char *port_str;
extern fd_set master_sockets, watch_sockets;
extern socklen_t remote_addr_length;


#endif /* SERVERUTILITY_H_ */

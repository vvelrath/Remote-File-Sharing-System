/*
 * ServerUtility.h
 *
 *  Created on: Sep 17, 2014
 *      Author: vivekanandh
 */

#ifndef SERVERUTILITY_H_
#define SERVERUTILITY_H_

//#include "../include/global.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

extern int max_socket;
extern int server_socket;
extern int listener;
extern int port_no;
extern int num_peers;
extern int num_conn;
extern char* port_str;
extern fd_set master_sockets, watch_sockets;
extern socklen_t remote_addr_length;

#endif /* SERVERUTILITY_H_ */

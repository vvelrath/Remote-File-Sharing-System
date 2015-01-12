/*
 * SelfInformation.h
 *
 *  Created on: Sep 16, 2014
 *      Author: vivekanandh
 */

#ifndef SELFINFORMATION_H_
#define SELFINFORMATION_H_

//#include "../include/global.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>


//Declarations
struct addrinfo *myaddrinfo_struct, *my_addrinfo_ptr;
char host_name[50];
struct sockaddr_in *my_address_info;
socklen_t* my_address_len;
char ip_str[INET_ADDRSTRLEN];
int ip_flag = 0;

extern int max_socket;
extern int listener;
extern int port_no;
extern char *port_str;
extern fd_set master_sockets, watch_sockets;

#endif /* SELFINFORMATION_H_ */

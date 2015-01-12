/*
 * Statistics.h
 *
 *  Created on: Sep 27, 2014
 *      Author: vivekanandh
 */

#ifndef STATISTICS_H_
#define STATISTICS_H_

//Declaring functions
void initializeStats();
void updateStatistics(int sender, char my_host_name[], char my_port_no[], char remote_host_name[], char remote_port_no[], int file_size, long double time_taken);
void sendStatisticsToServer(int sender, char my_host_name[], char my_port_no[], char remote_host_name[], char remote_port_no[], int file_size, long double time_taken);
void updateStatisticsInServer(int client_socket);
void displayStatistics();
void displayStatisticsInServer();
void displayStatisticsInClient();

#endif /* STATISTICS_H_ */

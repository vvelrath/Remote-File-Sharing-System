/**
 * File Name: SelfInformation.c
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

#include "../include/SelfInformation.h"
#include "../include/UtilityFunctions.h"


//Displays the creator information
void showCreatorInformation(){
	printf("\tName: Vivekanandh Vel Rathinam\n");
	printf("\tUBIT Name: vvelrath\n");
	printf("\tEmail Address: vvelrath@buffalo.edu\n");
	printf("\tI have read and understood the course academic integrity policy located at "
		   "\thttp://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity\n");
}


//Displays the input format of the available commands
void showHelperInformation(){
	fprintf(stdout,"-------------The below options are available-----------------\n");
	fprintf(stdout,"1. CREATOR\n");
	fprintf(stdout,"2. MYIP\n");
	fprintf(stdout,"3. MYPORT\n");
	fprintf(stdout,"4. MYIP\n");
	fprintf(stdout,"5. REGISTER <Server IP> <Server Port>\n");
	fprintf(stdout,"6. CONNECT <Peer IP> <Peer Port>\n");
	fprintf(stdout,"7. LIST\n");
	fprintf(stdout,"8. TERMINATE <Connection ID>\n");
	fprintf(stdout,"9. EXIT\n");
	fprintf(stdout,"10. UPLOAD <Connection ID> <Local file path>\n");
	fprintf(stdout,"11. DOWNLOAD <Conn ID1> <file name1> <Conn ID2> <file name2> <Conn ID3> <file name3>\n");
	fprintf(stdout,"12. STATISTICS\n\n");
}


//Displays the listening port
void showListeningPort(){
	printf("Port Number is %d\n", port_no);
}


//Displays the IP of the user
void computeMyIP(){

	//If the IP is not computed, compute it
	if(ip_flag == 0){
		//Getting the addrinfo data structure from our utility function
		myaddrinfo_struct = getAddrInfoStructure(host_name,NULL);

		for(my_addrinfo_ptr = myaddrinfo_struct; my_addrinfo_ptr != NULL; my_addrinfo_ptr = my_addrinfo_ptr->ai_next){
			// convert the sockaddr to a sockaddr_in via casting
			my_address_info = (struct sockaddr_in *)myaddrinfo_struct->ai_addr;

			// get the IP from the sockaddr_in and print it
			inet_ntop(PF_INET, &(my_address_info->sin_addr.s_addr), ip_str, INET_ADDRSTRLEN);
		}

		free(myaddrinfo_struct);

		ip_flag = 1;
	}else{
		fprintf(stdout,"\nMy IP Address is %s\n",ip_str);
	}
}

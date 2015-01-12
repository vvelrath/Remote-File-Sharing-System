Remote File Sharing System using C
===============================

### Description

Developed a simple application for file sharing among remote hosts and observed some network characteristics using it. Compared the results of this file sharing application with a standard tool, iperf, used for measuring network performance.

### Program Execution

1) Import the RemoteFSS folder into eclipse CDT as a makefile project and compile it													
2) Your process (your program when it is running in memory) will take 2 command line parameters:															

	1. The first parameter (s/c) indicates whether the program instance should run as a server or a client.													
	2. The second parameter (number) is the port number on which the process will listen for incoming connections.											

	● To run as a server listening on port 4322																						
	./assignment1 s 4322																								
	● To run as a client listening on port 4322																					
	./assignment1 c 4322																									
3) Use the HELP command for details on user interface options																							
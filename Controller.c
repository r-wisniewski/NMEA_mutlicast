/*************************************************************
*   Author: Robin Wisniewski, wisniewski.ro@gmail.com
*   Usage: ./Controller
*	
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	//Build argument vector to pass to NMEA_multicast
	char * const arg_vector[13] = { "./NMEA_multicast", "224.1.1.1", "11111", "192.168.0.20", "3", "GPGGA", "./NMEA_multicast", "224.1.1.1", "11111", "192.168.0.20", "2", "GNGNS", NULL};

	int PID;
	//fork a thread for each NMEA message to be output
	PID = fork();
	printf("%d\n", PID);
	if(PID == 0){
		//child code
		printf("%s", *arg_vector);
		execv("NMEA_multicast",arg_vector);
	}
	else{	
		if(fork() == 0){
			printf("%s", *(arg_vector+6));
			execv("NMEA_multicast",arg_vector+6);
		}
		else{
			//parent code
			int status;
			waitpid(PID,&status,0);
			printf("Child exit status: %d", status);
		}
	}

	return 0;
}


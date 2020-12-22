/*************************************************************
*   Author: Robin Wisniewski, wisniewski.ro@gmail.com
*   Usage: ./NMEA_Multicast [Local Interface IP Address]
*
*	Parameter(s):
*	- Local Interface IP Address: The IP address of the interface outputting the messages
*/

/****************************************************************************************** 
                COMPILE WITH cc NMEA_multicast.c -o NMEA_multicast -lpthread 
*******************************************************************************************/

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

//use a macro to define maximum buffer size
#define MAXBUFSIZE 256

struct arg_struct {
    int freq;
    char nmea[MAXBUFSIZE];
};

void *multicast_operation(void *arguments);

//NMEA-0183 format strings
char GPGGA[MAXBUFSIZE] = "$GPGGA,%.1f,%.8f,%c,%.8f,%c,%d,%d,%.1f,%.3f,%c,%.3f,%c,%.1f,%04d*%X\r\n"; 
char GPGNS[MAXBUFSIZE] = "$GPGNS,%.02f,%.5f,%c,%.5f,%c,%s,%d,%.1f,%.2f,%.2f,,%04d*%X\r\n"; 

//Declare global vars
char msg[MAXBUFSIZE];
time_t rawtime;
struct tm *info;
struct timespec ts;

char multicast_group[MAXBUFSIZE] = "224.1.1.1";
char multicast_port[MAXBUFSIZE] = "11111";
char local_ip[MAXBUFSIZE];

//for now the data is statically stored in varaibles here
// !!! No mutex protection for now !!!
char lat = 'N', lon = 'W', unit = 'M', mode[2] = "RR";
double latnum = 3723.46587704, lonnum = 2202.26957864;
float UTC = 172814.0, HDOP = 1.2, ortho_height = 18.893, Geoid_sep = -25.669, Age = 2.0;
int GPS_qual = 2, SV_in_use = 6, ref_station = 36, checksum = 79;

int main(int argc, char *argv[])
{	
	//set local interface IP
	strcpy(local_ip,argv[1]);

	//All are hardcoded for now, but with refinement can be input from a config file
	pthread_t thread_id1, thread_id2;

	struct arg_struct thread1;
	struct arg_struct thread2;

	thread1.freq = 2;
	strncpy(thread1.nmea,"GPGGA",5);

	thread2.freq = 3;
	strncpy(thread2.nmea,"GPGNS",5);

	int ret1;
	int ret2; 
	ret1 = pthread_create(&thread_id1, NULL, multicast_operation, (void *) &thread1);
	ret2 = pthread_create(&thread_id2, NULL, multicast_operation, (void *) &thread2);
	if(	ret1 != 0)
	{
		perror("Error: ");
	}
	if(	ret2 != 0)
	{
		perror("Error: ");
	}

	pthread_join(thread_id1, NULL);
	pthread_join(thread_id2, NULL);

	return 0;
}

void *multicast_operation(void *arguments){
	//Passing through argument structure and casting void arguments arg to an arg_struct
	struct arg_struct *args = (struct arg_struct *)arguments;
	
	// local interface is where we'll be sending data from
	// server_address is the mutlicast group
	struct in_addr localInterface;
	struct sockaddr_in server_address;

	//////////////**** Create Server socket ****//////////////
	//this is a multicast UDP socket
	int server_socket = socket(AF_INET,SOCK_DGRAM,0);
	//check for error
	if(server_socket == -1)
	{
		perror("Error: ");
	}
	else
	{
		printf("Socket successfully created\n");
	}
	//////////////**** Create the mutlicast group address structure  ****//////////////
	//this is used/passed into the connect function
	//set type of addr, port #, and ip addr
	server_address.sin_family = AF_INET;

	//port is supplied in the command line args
	//convert port # from string to int using atoi()
	//convert host byte order to network byte order using htons()
	//use 11111 to test
	server_address.sin_port = htons(atoi(multicast_port));

	//ip addr is supplied in the command line args
	//convert ip addr from string to int using inet_addr()
	//The multicast addresses are in the range 224.0.0.0 through 239.255.255.255
	//use 224.1.1.1 to test
	server_address.sin_addr.s_addr = inet_addr(multicast_group);

	//////////////**** Set the local interface we'll be spitting data out of ****//////////////
	localInterface.s_addr = inet_addr(local_ip);
	if(setsockopt(server_socket, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) == -1)
	{
		perror("Error: ");
	}
	else{
		printf("Successfully setup the local interface\n");
	}

	printf("Outputting $%s @ %dHz to %s:%s\n\n", args->nmea, args->freq, multicast_group, multicast_port);
	//////////////**** Send data to multicast group ****//////////////
	while(1){
		if(strncmp(args->nmea,"GPGGA",5) == 0) {
			//format the NMEA msg
			sprintf(msg,GPGGA,UTC,latnum,lat,lonnum,lon,GPS_qual,SV_in_use,HDOP,ortho_height,unit,Geoid_sep,unit,Age,ref_station,checksum);
			//nanosleep for appropriate time to meet frequency input
			if(args->freq == 1){
				sleep(1);
			}
			else{
				ts.tv_nsec = 1000000000/args->freq;
				nanosleep(&ts, &ts);
			}
		}
		else if(strncmp(args->nmea,"GPGNS",5) == 0){
			//format the NMEA msg
			sprintf(msg,GPGNS,UTC,latnum,lat,lonnum,lon,mode,SV_in_use,HDOP,ortho_height,Geoid_sep,Age,ref_station,checksum);
			//nanosleep for appropriate time to meet frequency input
			if(args->freq == 1){
				sleep(1);
			}
			else{
				ts.tv_nsec = 1000000000/args->freq;
				nanosleep(&ts, &ts);
			}
		}
		//Send out packet
		if(sendto(server_socket, msg, sizeof(msg), 0, (struct sockaddr*)&server_address, sizeof(server_address)) == -1){
			perror("Error: ");
		}
		else{
		time(&rawtime);
    	info = localtime(&rawtime);
		printf("Sent multicast packet \"%s\" at %s", msg, asctime(info));
		}
	}
}
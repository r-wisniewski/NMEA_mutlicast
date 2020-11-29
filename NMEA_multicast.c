/*************************************************************
*   Author: Robin Wisniewski, wisniewski.ro@gmail.com
*   Usage: ./NMEA_Multicast Multicast_IPv4_Addr Port_Number Local_Multicast_Interface
*	
*   Parameters:
*		- Multicast_IPv4_Addr: IPv4 mutlicast group address. Between 224.0.0.0 and 239.255.255.255
*		- Port_Number: Port number of the multicast group
*		- Local_Multicast_Interface: Local interface of server that outputs packets/messages to multicast group
*/

void errorfunc()
{
	//print out the error that occured
	perror("Error: ");
	exit(1);
};

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

//use a macro to define maximum buffer size
#define MAXBUFSIZE 1024

//NMEA-0183 format strings
char GPGGA[MAXBUFSIZE] = "$GPGGA,%.1f,%.8f,%c,%.8f,%c,%d,%d,%.1f,%.3f,%c,%.3f,%c,%.1f,%04d*%X"; 

int main(int argc, char *argv[])
{
	// local interface is where we'll be sending data from
	// server_address is the mutlicast group
	struct in_addr localInterface;
	struct sockaddr_in server_address;
	char msg[MAXBUFSIZE];
	time_t rawtime;
    struct tm *info;

	//Structure to run the server code is: ./NMEA_multicast Multicast_IPv4_Addr Port_Number Local_Multicast_Interface
    //check that the correct data is supplied
    if(argc < 3)
    {
        printf("Enter IP Address and Port number of server\n");
        exit(1);
    }

	//////////////**** Create Server socket ****//////////////
	//this is a multicast UDP socket
	int server_socket = socket(AF_INET,SOCK_DGRAM,0);
	//check for error
	if(server_socket == -1)
	{
		errorfunc();
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
	server_address.sin_port = htons(atoi(argv[2]));

	//ip addr is supplied in the command line args
	//convert ip addr from string to int using inet_addr()
	//The multicast addresses are in the range 224.0.0.0 through 239.255.255.255
	//use 224.1.1.1 to test
	server_address.sin_addr.s_addr = inet_addr(argv[1]);
	
	//////////////**** Set the local interface we'll be spitting data out of ****//////////////
	localInterface.s_addr = inet_addr(argv[3]);
	if(setsockopt(server_socket, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) == -1)
	{
		errorfunc();
	}
	else{
		printf("Successfully setup the local interface\n");
	}

	//////////////**** Send data to multicast group ****//////////////
	//for now the data is statically stored in varaibles here
	char lat = 'N', lon = 'W', unit = 'M';
	double latnum = 3723.46587704, lonnum = 2202.26957864;
	float UTC = 172814.0, HDOP = 1.2, ortho_height = 18.893, Geoid_sep = -25.669, Age = 2.0;
	int GPS_qual = 2, SV_in_use = 6, ref_station = 36, checksum = 79;
	while(1){
		//format the NMEA msg
		sprintf(msg, GPGGA ,UTC,latnum,lat,lonnum,lon,GPS_qual,SV_in_use,HDOP,ortho_height,unit,Geoid_sep,unit,Age,ref_station,checksum);
		//sleep for 1 second and output NMEA msg ~1Hz
		sleep(1);

		if(sendto(server_socket, msg, sizeof(msg), 0, (struct sockaddr*)&server_address, sizeof(server_address)) == -1){
		errorfunc();
		}
		else{
		time(&rawtime);
    	info = localtime(&rawtime);
		printf("Sent multicast packet \"%s\" at %s", msg, asctime(info));
		}
	}
	return 0;
}


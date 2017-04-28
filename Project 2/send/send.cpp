/* nonblock-udp_client.c */ 
/* Programmed by Adarsh Sethi */
/* February 19, 2017 */

/* Version of udp_client.c that is nonblocking */

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, sendto, and recvfrom */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <fcntl.h>          /* for fcntl */
#include <iostream>
#include <sstream>
#include <inttypes.h>

#define STRING_SIZE 1024

FILE * fileIn;
size_t bytesRead;
int fileIsOpen;
int packetNum;
unsigned long totalBytesSent;
int numOfResentPackets;
int totalDataPacketsSent;
int numACKs;
int timeoutCount;
long totalTimeTakenToTransmit;
int seqNum;
int nextSeqNum;
char packet[96];

using namespace std;

void initialize_variables(){
	
	packetNum = 0;
	totalBytesSent = 0;
	numOfResentPackets = 0;
	totalDataPacketsSent = 0;
	numACKs = 0;
	timeoutCount = 0;
	totalTimeTakenToTransmit = 0;
	seqNum = 0;
	nextSeqNum = 0;
}

int open_file(char filename[]){
	fileIn = fopen(filename, "rb");
	if (fileIn == NULL){
		cout << "\n\nError opening specified file. Please try again\n\n" << endl;
		return -1;
	}
	return 0;
}

int close_file(){
	fclose(fileIn);
}

int wait_for_call_from_above(char buffer[]){
	
	if (fgets (buffer , 80 , fileIn) == NULL){
		bytesRead = 0;
		return bytesRead;
	}
	else{
		bytesRead = strlen(buffer);
		cout << buffer;
	}
	
	return bytesRead;
}

int make_packet(char buffer[]){
	
	short bytesSent = strlen(buffer);
	uint16_t nBytesSent = htons(bytesSent);
	memcpy(&packet, &nBytesSent, 2);
	uint16_t seq = htons((uint16_t)seqNum);
	memcpy(packet + 2, &seq, 2);
	memcpy(packet + 4, &buffer, bytesSent);
	cout << packet << endl;
	
}

int main(void){
	
	char buffer[80];
	//char filename[1024];
	char *filename = "alice.txt";
	initialize_variables();
	/*
	do{
		cout << "\nPlease enter a file name (must include either absolute or relative path)" << endl;
		cin >> filename;
	
		fileIsOpen = open_file(filename);
	} while (fileIsOpen < 0);
	*/
	
	fileIsOpen = open_file(filename);
	//do{		
		wait_for_call_from_above(buffer);
		make_packet(buffer);
	//} while (bytesRead > 0);
		
	close_file();
	
	return 0;
}
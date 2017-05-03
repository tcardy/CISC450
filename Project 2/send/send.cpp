/* send.cpp */ 
/* Programmed by Adarsh Sethi
	modified by Tom Cardy*/
/* Spring 2017 */

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, sendto, and recvfrom */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <fcntl.h>          /* for fcntl */
#include <math.h>       	/* for exp */
#include <iostream>
#include <sstream>
#include <inttypes.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>


#define STRING_SIZE 1024

FILE * fileIn;   
size_t bytesRead;
int fileIsOpen;
int packetNum;
unsigned long totaldataBytesSent;
int numOfResentPackets;
int totalDataPacketsSent;
int numACKs;
int timeoutCount;
long totalTimeTakenToTransmit;
uint16_t seqNum;
int readNext;
int timeExpired;
char packet[92];
char ack_packet[1024];
char eot_packet[4];
struct itimerval timer;
long timeout_value;
int timeout_secs;
int timeout_usecs;
int exponent;
long duration;
short dataBytesSent;
struct timeval start, end;
int timeouts;
int bytes_sent, bytes_recd; /* number of bytes sent or received */
int sock_client;  /* Socket used by client */ 
struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
unsigned short client_port;  /* Port number used by client (local port) */
struct sockaddr_in server_addr;  /* Internet address structure that
                                       stores server address */
struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
char server_hostname[STRING_SIZE]; /* Server's hostname */
unsigned short server_port;  /* Port number used by server (remote port) */
unsigned int msg_len;  /* length of message */
int fcntl_flags; /* flags used by the fcntl function to set socket
                    for non-blocking operation */
socklen_t sock_len;
					
using namespace std;

/* open a socket */
void open_socket(){
   if ((sock_client = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      perror("Client: can't open datagram socket\n");
      exit(1);
   }
}

/* clear client address structure and initialize with client address */
void initialize_address(){
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* This allows choice of
                                        any host interface, if more than one 
                                        are present */
	client_addr.sin_port = htons(client_port);
}

// Binds the socket
void bind_socket(){
	
	if (bind(sock_client, (struct sockaddr *) &client_addr,
                                    sizeof (client_addr)) < 0) {
      perror("Client: can't bind to local address\n");
      close(sock_client);
      exit(1);
   }

   /* make socket non-blocking */
   fcntl_flags = fcntl(sock_client, F_GETFL, 0);
   fcntl(sock_client, F_SETFL, fcntl_flags | O_NONBLOCK);
}

// Initializes the server
void initialize_server(){
	/*
	printf("Enter hostname of server: ");
	scanf("%s", server_hostname);
	*/
	strcpy(server_hostname, "localhost");
	if ((server_hp = gethostbyname(server_hostname)) == NULL) {
		perror("Client: invalid server hostname\n");
		close(sock_client);
		exit(1);
	}
	/*
	printf("Enter port number for server: ");
	scanf("%hu", &server_port);
	*/
	server_port = 5045;

	/* Clear server address structure and initialize with server address */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
	server_addr.sin_port = htons(server_port);
	
}

// Signal handler for timeout events
void sigalrm_handler(int sig)
{
    timeExpired = 1;
	timeoutCount++;
	timeouts++;
	cout << "Timeout expired for packet numbered " << seqNum <<endl;
	signal(SIGALRM, sigalrm_handler);  // Ready the handler for another signal
}

// Sets the timeout timeout
void set_timer(){
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	timer.it_value.tv_sec = timeout_secs; 
	timer.it_value.tv_usec = timeout_usecs;
	setitimer(ITIMER_REAL, &timer,0);
	timeExpired = 0;
}

// Stops the timer when ACK is recv'd 
void stop_timer(){
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	timer.it_value.tv_sec = 0; 
	timer.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &timer,0);
	timeExpired = 0;
}

// Initializes global variables
void initialize_variables(){
	
	packetNum = 0;
	totaldataBytesSent = 0;
	numOfResentPackets = 0;
	totalDataPacketsSent = 0;
	numACKs = 0;
	timeoutCount = 0;
	totalTimeTakenToTransmit = 0;
	seqNum = (uint16_t)0;
	sock_len = 0;
	timeouts = 0;
}

// Opens the input file
int open_file(char filename[]){
	fileIn = fopen(filename, "rb");
	if (fileIn == NULL){
		cout << "\n\nError opening specified file. Please try again\n\n" << endl;
		return -1;
	}
	return 0;
}

// closes input
int close_file(){
	fclose(fileIn);
}

// Simulates calls from upper level by reading from file
int wait_for_call_from_above(char buffer[]){
	
	if (fgets (buffer , 80 , fileIn) == NULL){
		bytesRead = 0;
		return bytesRead;
	}
	else{
		bytesRead = strlen(buffer);
	}
	return bytesRead;
}

// Examines the ACK recv'd
int read_ack(char ack_packet[]){
	
	uint16_t ack;
    memcpy(&ack, ack_packet, 2);
    uint16_t returnedSeq = ntohs(ack);
	if (returnedSeq == seqNum){
		readNext = 1;
		numACKs++;
		cout << "ACK " << seqNum << " received" << endl;
		seqNum = (uint16_t)(1 - seqNum);
	}
	else{
		bytes_recd = 0;
		set_timer();
	}
}

// Read ACK from buffer
void recv_ack(){
	
	bytes_recd = recvfrom(sock_client, ack_packet, 1024, 0,
			(struct sockaddr *) 0, &sock_len);
			
	if (bytes_recd > 0){
		read_ack(ack_packet);
	}
}

// Sends a packet to the receiver
int send_packet(char packet[]){
	
	msg_len = dataBytesSent + 5;
	bytes_sent = sendto(sock_client, packet, msg_len, 0,
        (struct sockaddr *) &server_addr, sizeof (server_addr));
	cout << "Packet " << seqNum << " transmitted with " << bytes_sent - 5 << " data bytes" << endl;
	totaldataBytesSent += (bytes_sent - 5);
}

// Resends the data packet due to timeout
int resend_packet(char packet[]){
	msg_len = dataBytesSent + 5;
	bytes_sent = sendto(sock_client, packet, msg_len, 0,
        (struct sockaddr *) &server_addr, sizeof (server_addr));
	cout << "Packet " << seqNum << " retransmitted with " << bytes_sent - 5 << " data bytes" << endl;
	numOfResentPackets++;
}

// Readies the packet for sending
int make_packet(char buffer[]){
	
	dataBytesSent = strlen(buffer) + 1;
	uint16_t ndataBytesSent = htons(dataBytesSent);
	memcpy(&packet, &ndataBytesSent, 2);
	uint16_t seq = htons(seqNum);
	memcpy(packet + 2, &seq, 2);
	memcpy(packet + 4, buffer, dataBytesSent);
}

// Transmits the EOT packet
void send_eot(){
	
	uint16_t eotCount = htons((uint16_t)0);
	memcpy(&eot_packet, &eotCount, 2);
	uint16_t seq = htons(seqNum);
	memcpy(eot_packet + 2, &seq, 2);
	bytes_sent = sendto(sock_client, eot_packet, 4, 0,
        (struct sockaddr *) &server_addr, sizeof (server_addr));
	cout << "End of Transmission Packet with sequence number ";
	cout << seqNum << " transmitted with 0 data bytes" << endl;
}

// Calculates the timeout value based upon user's input
void determine_timeout_values(){
	timeout_value = pow(10, exponent);
	timeout_secs = (timeout_value / 1000000);
	timeout_usecs = (timeout_value - (timeout_secs * 1000000));
}

// Waits until ACK is recv'd or timeout
void wait_for_ack(){
	
	while(1){
		recv_ack();
		if (bytes_recd > 0){
			stop_timer();
			break;
		} 
		if (timeExpired > 0){
			break;
		}
	}
}

/* Clears the recv buffer in case of duplicate ACK from duplicate 
	transmissions */
void clear_recv_buffer(){
	int bytesAv = 0;
	
	while ( bytesAv >= 1 )
	{
		bytes_recd = recvfrom(sock_client, ack_packet, bytesAv, 0,
			(struct sockaddr *) 0, &sock_len);
			
		ioctl (sock_client,FIONREAD,&bytesAv);
	}
	ioctl (sock_client,FIONREAD,&bytesAv);
	
	bytes_recd = 0;
}

// Resets critical values for next iteration
void next_iteration(){
	
	timeExpired = 0;
	readNext = 0;
	bytes_sent = 0;
	bytes_recd = 0;
	packetNum++;
}

// Prints the EOT report
void print_report(){
	
	cout << "Number of data packets transmitted: " << packetNum << endl;
	cout << "Total number of data bytes transmitted: " << totaldataBytesSent << endl;
	cout << "Total number of retransmissions: " << numOfResentPackets << endl;
	cout << "Total number of data packets transmitted: " << numOfResentPackets + packetNum << endl;
	cout << "Number of ACKs received: " << numACKs << endl;
	cout << "Number of times timeout expired: " << timeoutCount << endl;
	cout << "Time, in milliseconds, taken to transmit file: " << duration << endl;
}

// Converts secs and usecs to msecs
void calculate_duration(){
	duration = ((end.tv_sec * 1000 + end.tv_usec / 1000)
		  - (start.tv_sec * 1000 + start.tv_usec / 1000));
}

int main(void){
	
	client_port = 0; 
	open_socket();
	initialize_address();
	bind_socket();
	initialize_server();
	char buffer[80];
	char filename[1024];
	initialize_variables();
	signal(SIGALRM, sigalrm_handler); // Readies the signal handler for first signal

	cout << "Please enter a timeout exponent (Between 1 and 10):\n" << endl;
	cin >> exponent;
	
	// Loops until user inputs a proper value for timeout exponent
	while (cin.fail() || exponent > 10 || exponent < 1){
		cout << "Please enter a number between 1 and 10\n" << endl;
		cin.clear(); 
		cin.ignore(256,'\n'); 
		cin >> exponent;
	}
	determine_timeout_values();
	
	// Loops until user enters a valid path to input file
	do{
		cout << "\nPlease enter a file name (must include either absolute or relative path)" << endl;
		cin >> filename;
	
		fileIsOpen = open_file(filename);
	} while (fileIsOpen < 0);
	
	// Starts the duration timer
	gettimeofday(&start, NULL);
	// Main loop: sends packets to recv program until input file ends
	do{		
		wait_for_call_from_above(buffer);
		make_packet(buffer);
		if (timeouts > 0){
			clear_recv_buffer();
			timeouts = 0;
		}
		send_packet(packet);
		set_timer();
		wait_for_ack();
		while(readNext == 0){
			if (timeExpired > 0){
				resend_packet(packet);
				set_timer();
				wait_for_ack();
			}
		}
		next_iteration();
		
		
	} while (bytesRead > 0);
	
	// Final cleanup and reporting
	send_eot();
	gettimeofday(&end, NULL);
	calculate_duration();
	print_report();
	close_file();
	close (sock_client);
	
	return 0;
}
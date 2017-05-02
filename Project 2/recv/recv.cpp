/* udp_server.c */
/* Programmed by Adarsh Sethi */
/* February 19, 2017 */


#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, sendto, and  recvfrom */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <fcntl.h>
#include <inttypes.h>
#include <fstream>
#define STRING_SIZE 1024
#include <ctime>
/* SERV_UDP_PORT is the port number on which the server listens for
   incoming messages from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_UDP_PORT 5045

using namespace std;

ofstream outFile;
short expected_seq;
short count;
int numPackets;
int totalBytes;
int sock_server;  /* Socket on which server listens to clients */

struct sockaddr_in server_addr;  /* Internet address structure that
                                    stores server address */
unsigned short server_port;  /* Port number used by server (local port) */
struct sockaddr_in client_addr;  /* Internet address structure that
                                    stores client address */
unsigned int client_addr_len;  /* Length of client address structure */
char sentence[STRING_SIZE];  /* receive message */
char modifiedSentence[STRING_SIZE]; /* send message */
unsigned int msg_len;  /* length of message */
int bytes_sent, bytes_recd; /* number of bytes sent or received */
unsigned int i;  /* temporary loop variable */

void init_variables(){
	numPackets = 0;
	totalBytes = 0;
}
/* open a socket */
void open_socket(){
	if ((sock_server = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("Server: can't open datagram socket\n");
		exit(1);
	}
}

/* initialize server address information */
void init_server(){
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */
	server_port = SERV_UDP_PORT; /* Server will listen on this port */
	server_addr.sin_port = htons(server_port);
}

/* bind the socket to the local server port */
void bind_socket(){
	if (bind(sock_server, (struct sockaddr *) &server_addr,
      sizeof (server_addr)) < 0) {
		perror("Server: can't bind to local address\n");
		close(sock_server);
		exit(1);
	}
}

void open_file(){
	outFile.open("out.txt", ios::out);
}

void close_file(){
	outFile.close();
}

void write_to_file(char buffer[]){ 
	outFile << buffer;
}

int simulate_packet_loss(double rate){

	/*This function will generate a random number between 0-1 and return a 0 or
	1, based on if the random number is larger than the loss rate. This function is
	useful enough for both packet loss and ack loss*/
	double x =  rand() / (RAND_MAX + 1.);
		
	if (x < rate)
		return 0;	
	else
		return 1;
}

int main(void) {
	double packet_loss_rate; //Parameter to Drop Packet
	double ack_loss_rate;	//Parameter to Drop Ack
	srand (time(NULL)); // Seeding a Random Number
	expected_seq = 0; //The seq number to be expected
	
	printf("Please enter Packet loss Rate. Example: .2 \n\n"); //User enters packet loss rate
	cin >> packet_loss_rate;	
	while (cin.fail() || packet_loss_rate < 0 || packet_loss_rate > 1){
		cout << "Please enter a number between 0 and 1\n" << endl;
		cin.clear(); 
		cin.ignore(256,'\n'); 
		cin >> packet_loss_rate;
	}
	
	printf("Please enter Ack loss rate. Example: .3 \n\n"); //User enters act loss rate
	cin >> ack_loss_rate;
	while (cin.fail() || ack_loss_rate < 0 || ack_loss_rate > 1){
		cout << "Please enter a number between 0 and 1\n" << endl;
		cin.clear(); 
		cin.ignore(256,'\n'); 
		cin >> ack_loss_rate;
	}
	open_socket();
	init_server();
	bind_socket();
	open_file();
	char buffer[82];
	char packet[92];
	char ack[6];
	
	/* wait for incoming messages in an indefinite loop */

	printf("Waiting for incoming messages on port %hu\n\n", 
		server_port);

	client_addr_len = sizeof (client_addr);


	while (1) {

		bytes_recd = recvfrom(sock_server, &packet, 92, 0,
            (struct sockaddr *) &client_addr, &client_addr_len);
		//Get the Count
		uint16_t container;
		memcpy(&container, &packet, 2);
		uint16_t converter = ntohs(container);
		short count = (short)converter;
		
		
		memcpy(&container, packet + 2, 2);
		converter = ntohs(container);
		short seq = (short)converter;
		
		//If count is 0, end program	
		if (count == 0){
			cout << "End of Transmission Packet with sequence number " << seq ;
			cout << " received with 0 data bytes" << endl;
			break;
		}
		
		//If sequence number is correct, continue
		if (seq == expected_seq){
			int loss_packet = simulate_packet_loss(packet_loss_rate);
			//If packet is not loss, continue
			if (loss_packet == 1){
				//Write File
		
				cout << "Packet " << seq << " received with " << count << " data bytes" <<endl;
				memcpy(buffer, packet + 4, count);
				write_to_file(buffer);
		
				//Changes the expected sequence number
				expected_seq = 1 - expected_seq;
				
				int loss_ack = simulate_packet_loss(ack_loss_rate);
				//If Ack is not to be lost, send message
		
				if (loss_ack == 1){
					/* prepare the message to send */

					msg_len = 2;
					uint16_t ackSeq = htons((uint16_t)seq);
					memcpy(ack, &ackSeq, 2);
					
					/* send ACK */
					cout << "ACK " << seq << " transmitted" << endl;
					bytes_sent = sendto(sock_server, ack, msg_len, 0,
						(struct sockaddr*) &client_addr, client_addr_len);
					
			
				}
				else{
					cout << "ACK " << seq << " lost" << endl;
				}
			}
			else{
				cout << "Packet " << seq << " lost" << endl;
			}
		}
		// Duplicate received
		else{
			cout << "Duplicate packet " << seq << " received with " << count << " data bytes" << endl;
			cout << "ACK " << seq << " transmitted" << endl;
			msg_len = 2;
			uint16_t ackSeq = htons((uint16_t)seq);
			memcpy(ack, &ackSeq, 2);
			/* send message */
			
			bytes_sent = sendto(sock_server, ack, msg_len, 0,
				(struct sockaddr*) &client_addr, client_addr_len);
		}
	}
	close_file();
}

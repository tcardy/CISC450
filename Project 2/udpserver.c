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

void filewrite(char text[]){ 
	//This Function is to be used for file writing
	//Not Supported by GCC, needs to be changed
	//ofstream file;
//	file.open("text.txt", ios::in | ios::app);
//	int sizeofArray = (sizeof(text)/sizeof(*text)
//	for (int i = 0; i < sizeofArray; i++){
//	file << text[i];
//	}
//	file.close();

}
int simulate_packet_loss(double rate){

	/*This function will generate a random number between 0-1 and return a 0 or
	1, based on if the random number is larger than the loss rate. This function is
	useful enough for both packet loss and ack loss*/
	double x =  rand() / (RAND_MAX + 1.);
		
	printf("%lf", x);
	if (x < rate) {
	return 0;
	}	
	
	else
	{
	return 1;
	}
}
int main(void) {
double packet_loss_rate; //Parameter to Drop Packet
double ack_loss_rate;	//Parameter to Drop Ack
srand (time(NULL)); // Seeding a Random Number
int expected_seq = 0; //The seq number to be expected
	printf("Please enter Packet loss Rate. Example: .2 \n\n"); //User enters packet loss rate
	std::cin >> packet_loss_rate;	
	printf("Please enter Ack loss rate. Example: .3 \n\n"); //User enters act loss rate
	std::cin >> ack_loss_rate;
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

   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      perror("Server: can't open datagram socket\n");
      exit(1);
   }

   /* initialize server address information */

   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */
   server_port = SERV_UDP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address\n");
      close(sock_server);
      exit(1);
   }

   /* wait for incoming messages in an indefinite loop */

   printf("Waiting for incoming messages on port %hu\n\n", 
                           server_port);

   client_addr_len = sizeof (client_addr);


   for (;;) {

      bytes_recd = recvfrom(sock_server, &sentence, STRING_SIZE, 0,
                     (struct sockaddr *) &client_addr, &client_addr_len);
      printf("Received Sentence is: %s\n     with length %d\n\n",
                         sentence, bytes_recd);
	//Get the Count
	uint16_t temp;
	memcpy(&temp, &sentence, 2);
	uint16_t temp3 = ntohs(temp);
	//If count is 0, end program	
	if (temp3 == 0){
	printf("End of Transmission Packet has Arrived, Terminating Program");	
	exit(0);
	}
	memcpy(&temp, &sentence + 2, 2);
	temp3 = ntohs(temp);
	//If sequence number is correct, continue
	if (temp3 == expected_seq){
	int loss_packet = simulate_packet_loss(packet_loss_rate);
	//If packet is not loss, continue
	if (loss_packet == 1){
	//Write File
	filewrite(sentence);
	int loss_ack = simulate_packet_loss(ack_loss_rate);
	//If Ack is not to be lost, send message
	if (loss_ack == 1){
		 /* prepare the message to send */

      msg_len = bytes_recd;
      for (i=0; i<msg_len; i++)
         modifiedSentence[i] = toupper (sentence[i]);

      /* send message */
 
      bytes_sent = sendto(sock_server, modifiedSentence, msg_len, 0,
               (struct sockaddr*) &client_addr, client_addr_len);
	//Changes the expected sequence number
	if (expected_seq == 0){
	expected_seq == 1;
	}
	else{
	expected_seq == 0;	
	}
	}
	
	}
	}
   }
}

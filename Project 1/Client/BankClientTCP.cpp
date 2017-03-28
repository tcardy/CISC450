/* tcp_ client.c */
/* Programmed by Adarsh Sethi */
/* February 19, 2017 */

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string>
#include <bitset>
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
using namespace std;

#define STRING_SIZE 1024
//This function does a simple Int to Char Conversion
char Int_to_Char_conversion(int number){
	char digits[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	char achar = digits[number];
	return achar;
}
//This function tailors the message to ask for a balance
void Balance_Request(char sentence[], int Account, int Transaction_Type){
	sentence[0] = '0';
	sentence[1] = Int_to_Char_conversion(Transaction_Type);
	sentence[2] = Int_to_Char_conversion(Account);
	sentence[3] = Int_to_Char_conversion(Account);
	sentence[4] = '\o';

	//Just need to send Sentence at this point
}
//This function Rounds any number by extracting the Quotient by division and returning an acceptable amount
int Rounded_Amount(int money){
	int Quotient = money / 20; //increments of 20
	return Quotient * 20; //Returns the amount, minus the remainder
}
//This function handles all transactions regarding money.
void Money_Transactions(char sentence[], int Account, int Transaction_Type, int Transfer_Destination){
	int money = 0;

	sentence[0] = '0'; //start with 0 for error flag
	sentence[1] = Int_to_Char_conversion(Transaction_Type); // Fill index with what Transaction
	sentence[2] = Int_to_Char_conversion(Account); //Fill index with Which Account
	sentence[3] = Int_to_Char_conversion(Transfer_Destination); // Fill index with Account Transfer Destination

	cin >> money; //User inputs amount
	std::bitset<20> b(money); //convent number into bit array
	string test = b.to_string();
	int j = 0; //Iterator for the String
	for (int i = 4; i < 24; i++){ //For loop that loads our Bitset into the Char Array
		sentence[i] = test[j];
		j++;
	}


	for (int k = 0; k < 24; k++){
		cout << sentence[k];
	}
	cout << endl;
	//Message is ready to send
	//send message here
}
int main(void) {

	int sock_client;  /* Socket used by client */
	struct sockaddr_in server_addr;  /* Internet address structure that stores server address */
	struct hostent * server_hp;      /* Structure to store server's IP address */
	char server_hostname[STRING_SIZE]; /* Server's hostname */
	unsigned short server_port;  /* Port number used by server (remote port) */
	char sentence[STRING_SIZE];  /* send message */
	char modifiedSentence[STRING_SIZE]; /* receive message */
	unsigned int msg_len;  /* length of message */
	int bytes_sent, bytes_recd; /* number of bytes sent or received */

	//My New Variables
	int Switch_Menu; //The Menu Iterator
	int Account_Type; //The Account the User wants to interact with
	int Transfer_Destination; //Variable used for Transfer Transactions
	/* open a socket */
	if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("Client: can't open stream socket");
		exit(1);
	}

	/* Note: there is no need to initialize local client address information
	unless you want to specify a specific local port
	(in which case, do it the same way as in udpclient.c).
	The local address initialization and binding is done automatically
	when the connect function is called later, if the socket has not
	already been bound. */

	/* initialize server address information */

	printf("Enter hostname of server: ");
	scanf("%s", server_hostname);
	if ((server_hp = gethostbyname(server_hostname)) == NULL) {
		perror("Client: invalid server hostname");
		close(sock_client);
		exit(1);
	}

	printf("Enter port number for server: ");
	scanf("%hu", &server_port);

	/* Clear server address structure and initialize with server address */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
		server_hp->h_length);
	server_addr.sin_port = htons(server_port);

	/* connect to the server */

	if (connect(sock_client, (struct sockaddr *) &server_addr,
		sizeof(server_addr)) < 0) {
		perror("Client: can't connect to server");
		close(sock_client);
		exit(1);
	}

	/* user interface */

	//printf("Please input a sentence:\n");
	//scanf("%s", sentence);
	//msg_len = strlen(sentence) + 1;
	//My Stuff
	do {
		cout << "What would you like to do today?" << endl;
		cin >> Switch_Menu; // User inputs what they want to do via switch statement

		switch (Switch_Menu) {
		case 0: //Check Balance Option
			cout << "Check Balance" << endl;
			cout << "Enter your Account: 0 for Checking, 1 for Savings" << endl;
			cin >> Account_Type;
			Transfer_Destination = Account_Type;
			Balance_Request(sentence, Account_Type, Switch_Menu); //Function that fills sentence to be sent
			bytes_sent = send(sock_client, sentence, msg_len, 0);
			break;
		case 1: //Deposit Option
			cout << "Deposit" << endl;
			cout << "Enter your Account: 0 for Checking, 1 for Savings" << endl;
			cin >> Account_Type;
			Transfer_Destination = Account_Type;
			cout << "How much would you like to deposit?" << endl;
			Money_Transactions(sentence, Account_Type, Switch_Menu, Transfer_Destination); //Function that fills sentence to be sent
			bytes_sent = send(sock_client, sentence, msg_len, 0);
			break;
		case 2:
			cout << "Withdrawl" << endl;
			cout << "Enter your Account: 0 for Checking, 1 for Savings" << endl;
			cin >> Account_Type;
			Transfer_Destination = Account_Type;
			cout << "How much would you like to Withdraw?" << endl;
			Money_Transactions(sentence, Account_Type, Switch_Menu, Transfer_Destination);
			bytes_sent = send(sock_client, sentence, msg_len, 0);
			break;
		case 3:
			cout << "Transfer" << endl;
			cout << "Enter The Account Source: 0 for Checking, 1 for Saving" << endl;
			cin >> Account_Type;
			cout << "Enter The Account Destination: 0 for Checking, 1 for Saving" << endl;
			cin >> Transfer_Destination;
			cout << "How much would you like to Transfer? Whole Dollars only" << endl;
			Money_Transactions(sentence, Account_Type, Switch_Menu, Transfer_Destination);
			bytes_sent = send(sock_client, sentence, msg_len, 0);
			break;
		case 4:
			cout << "Closing Program" << endl;
			break;
		default:
			cout << "Invalid Option" << endl;
		}

		cout << "Transactions Completed " << Switch_Menu << endl;
	} while (Switch_Menu != 4);

	//End of My Stuff
	/* send message */

	//bytes_sent = send(sock_client, sentence, msg_len, 0);

	/* get response from server */

	bytes_recd = recv(sock_client, modifiedSentence, STRING_SIZE, 0);

	printf("\nThe response from server is:\n");
	printf("%s\n\n", modifiedSentence);

	/* close the socket */

	close(sock_client);
}

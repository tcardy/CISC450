/* bankserver.cpp */
/* Programmed by Adarsh Sethi
	modified by Tom Cardy*/
/* Spring 2017 */    

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, bind, listen, accept */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <bitset>  			/* for bitset */
#include <iostream>			/* for cout*/

#define STRING_SIZE 1024   

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_TCP_PORT 5045

using namespace std;

class Account {
   public:
		int account;   // Account number
		char type;	// Acount type (Checking or savings)
		int balance;   // The current balance of the account
	  
	int deposit(int amount){
		balance += amount;
		return balance;
	}
	  
	int withdraw(int amount){
		
		if (type != 'c' )
			return -1;
		else if (amount > balance)
			return -2;
		else if (amount % 20 != 0)
			return -3;
		else
			balance -= amount;
		
		return balance;
	}	

	int check_balance(void){
			
		return balance;
	}
		
	int transfer(Account& to, int amount){
		if (balance >= amount){
			to.balance += amount;
			balance -= amount;
			return balance;
		}
		return -4;
	}
};

void init_account(Account& a, int arg1, char arg2, int arg3){
		a.account = arg1;
		a.type = arg2;
		a.balance = arg3;
}

// Account variables
Account checking;
Account savings;

// Global communication variables

void change_amount_sentence(char sentence[], int amount, int index){
	bitset<20> b1 (amount);
	string s = b1.to_string();
	
	int n = s.length();
	int j;
	for (j = 0; j < n; j++)
		sentence[index + j] = s[j];
	sentence[index + j] = '\0';
}

void process_check_balance(char sentence[]){
	int value;
	
	if (sentence[2] == '0')
		value = checking.check_balance();
	
	else if (sentence[2] == '1')
		value = savings.check_balance();
		
	change_amount_sentence(sentence, value, 4);
	
}

void process_withdraw(char sentence[]){
	
	string s(sentence + 4, sentence + 24);
	bitset<20> b1 (s);
	int returncode;
	
	if (sentence[2] == '0'){
		returncode = checking.withdraw((int) b1.to_ulong());
		if (returncode < 0){
			if (abs(returncode) == 1)
				sentence[0] = '1';
			else if (abs(returncode) == 2)
				sentence[0] = '2';
			else if (abs(returncode) == 3)
				sentence[0] = '3';
		}
		else{
			change_amount_sentence(sentence, returncode, 4);
		}
	}
	else if (sentence[2] == '1'){
		returncode = savings.withdraw((int) b1.to_ulong());
		if (returncode < 0){
			if (abs(returncode) == 1)
				sentence[0] = '1';
			else if (abs(returncode) == 2)
				sentence[0] = '2';
			else if (abs(returncode) == 3)
				sentence[0] = '3';
		}
		else{
			change_amount_sentence(sentence, returncode, 4);
		}
	}
}

void process_deposit(char sentence[]){
	string s(sentence + 4, sentence + 24);
	bitset<20> b1 (s);
	int returncode = 0;
	
	if (sentence[2] == '0'){
		returncode = checking.deposit((int) b1.to_ulong());
	}
	else if (sentence[2] == '1'){
		returncode = savings.deposit((int) b1.to_ulong());	
	}
	
	if (returncode > 0){
		change_amount_sentence(sentence, returncode, 4);
	}
}

void process_transfer(char sentence[]){
	string s(sentence + 4, sentence + 24);
	bitset<20> b1 (s);
	int returncode = 0;
	
	if (sentence[2] == '0' && sentence[3] == '1'){
		returncode = checking.transfer(savings, (int) b1.to_ulong());
		if (returncode > 0){
			change_amount_sentence(sentence, checking.balance, 4);
			change_amount_sentence(sentence, savings.balance, 24);
		}
		else{
			sentence[0] = '4';
		}
	}
	else if (sentence[2] == '1' && sentence[3] == '0'){
		returncode = savings.transfer(checking, (int) b1.to_ulong());	
		if (returncode > 0){
			change_amount_sentence(sentence, savings.balance, 4);
			change_amount_sentence(sentence, checking.balance, 24);
		}
		else{
			sentence[0] = '4';
		}
	}
}

void process_transaction(char sentence[]){
	
	cout << "Processing transaction\n" << endl;
	
	if (sentence[1] == '0')
		process_check_balance(sentence);
	else if (sentence[1] == '1')
		process_deposit(sentence);
	else if (sentence[1] == '2')
		process_withdraw(sentence);
	else if (sentence[1] == '3')
		process_transfer(sentence);	
}


int main(void){
	
	init_account(checking, 1, 'c', 0);
	init_account(savings, 2, 's',0);
	
	int sock_server;  /* Socket on which server listens to clients */
	int sock_connection;  /* Socket on which server exchanges data with client */
	struct sockaddr_in server_addr;  /* Internet address structure that stores server address */
	unsigned int server_addr_len;  /* Length of server address structure */
	unsigned short server_port;  /* Port number used by server (local port) */
	struct sockaddr_in client_addr;  /* Internet address structure that stores client address */
	unsigned int client_addr_len;  /* Length of client address structure */
	char sentence[STRING_SIZE];  /* receive message */
	char modifiedSentence[STRING_SIZE]; /* send message */
	unsigned int msg_len;  /* length of message */
	int bytes_sent, bytes_recd; /* number of bytes sent or received */
	
	/* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Server: can't open stream socket");
      exit(1);                                                
   }

   /* initialize server address information */
    
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */ 
   server_port = SERV_TCP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }                     

   /* listen for incoming requests from clients */

   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   printf("I am here to listen ... on port %hu\n\n", server_port);
  
   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */
   
   sock_connection = accept(sock_server, (struct sockaddr *) &client_addr, 
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
    if (sock_connection < 0) {
       perror("Server: accept() error\n"); 
       close(sock_server);
       exit(1);
    }
	
	while(1){
 
      /* receive the message */

      bytes_recd = recv(sock_connection, sentence, STRING_SIZE, 0);

      if (bytes_recd > 0){
         printf("Received Sentence is:\n");
         printf("%s", sentence);
         printf("\nwith length %d\n\n", bytes_recd);

        /* prepare the message to send */

		 process_transaction(sentence);
		 msg_len = strlen(sentence);

         /* send message */
 
         bytes_sent = send(sock_connection, sentence, msg_len, 0);
		}
	}
	close(sock_connection);
	return 0;
	
}

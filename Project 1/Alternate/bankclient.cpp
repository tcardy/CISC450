/* bankclient.cpp */ 
/* Programmed by Adarsh Sethi
	modified by Tom Cardy*/
/* Spring 2017 */     

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <bitset>  			/* for bitset */
#include <iostream>			/* for cout*/

#define STRING_SIZE 1024

using namespace std;

int displayMainMenu(char sentence[]){
	int choice;
	cout << "Welcome to the main menu\nPlease select from one of the following options:\n" << endl;
	cout << "0. Check Balance\n1. Deposit\n2. Withdraw\n3. Transfer Between Accounts\n4. Exit" << endl;
	cin >> choice;
	
	while (cin.fail() || choice > 4 || choice < 0){
		cout << "Please enter a number from the menu\n" << endl;
		cin.clear();
		cin.ignore(256,'\n');
		cin >> choice;
	}
	
	if (choice == 4){
		return 1;
	}
	
	sentence[0] = '0';
	switch (choice){
		case 0 :	sentence[1] = '0';
					break;
		case 1 :    sentence[1] = '1';
					break;
		case 2 :	sentence[1] = '2';
					break;
		case 3 :	sentence[1] = '3';
					break;
	}
	return 0;
}

int accountSelectMenu(string s){
	int choice;
	
	cout << "Please select an account for " << s << "\n" << endl;
	cout << "0. Checking\n1. Savings\n" << endl;
	cin >> choice;
	
	while (cin.fail() || choice > 1 || choice < 0){
		cout << "Please enter either 0 or 1\n" << endl;
		cin.clear();
		cin.ignore(256,'\n');
		cin >> choice;
	}
	
	return choice;
}

int getAmount(){
	int choice;
	
	cout << "Please enter an amount in whole dollars\n" << endl;
	cin >> choice;
	
	while (cin.fail() || choice > 1000000 || choice < 0){
		cout << "Please enter a valid amount\n" << endl;
		cin.clear();
		cin.ignore(256,'\n');
		cin >> choice;
	}
	
	return choice;
}

void change_amount_sentence(char sentence[], int amount, int index){
	bitset<20> b1 (amount);
	string s = b1.to_string();
	
	int n = s.length();
	int j;
	for (j = 0; j < n; j++)
		sentence[index + j] = s[j];
	sentence[index + j] = 0;
}

int get_amount_sentence(char mSentence[], int index){
	
	string s(mSentence + index, mSentence + (index + 20));
	bitset<20> b1 (s);
	
	int num = (int) b1.to_ulong();
	
	return num;
}

void processCheckBalance(char sentence[]){
	int acct = accountSelectMenu("balance check");
	
	if (acct == 0)
		sentence[2] = '0';
	else if (acct == 1)
		sentence[2] = '1';
	
	sentence[3] = '0';
	sentence[4] = 0;
}

void processDeposit(char sentence[]){
	int acct = accountSelectMenu("deposit");
	
	if (acct == 0)
		sentence[2] = '0';
	else if (acct == 1)
		sentence[2] = '1';
	
	sentence[3] = '0';
	
	int amt = getAmount();
	change_amount_sentence(sentence, amt, 4);
}

void processWithdraw(char sentence[]){
	int acct = accountSelectMenu("withdraw");
	
	if (acct == 0)
		sentence[2] = '0';
	else if (acct == 1)
		sentence[2] = '1';
	
	sentence[3] = '0';
	
	int amt = getAmount();
	change_amount_sentence(sentence, amt, 4);
}

void processTransfer(char sentence[]){
	int src;
	int dst;
	
	while (1){
		
		src = accountSelectMenu("transfer source");
		
		if (src == 0)
			sentence[2] = '0';
		else if (src == 1)
			sentence[2] = '1';
		
		dst = accountSelectMenu("transfer destination");
		
		if (dst == 0)
			sentence[3] = '0';
		else if (dst == 1)
			sentence[3] = '1';
		
		if (dst == src){
			cout << "Please ensure that the source and destination accounts are not the same" << endl;
		}
		
		else 
			break;
	}
	
	int amt = getAmount();
	
	change_amount_sentence(sentence, amt, 4);
}

void processRequest(char sentence[]){
	
	if (sentence[1] == '0')
		processCheckBalance(sentence);
	else if (sentence[1] == '1')
		processDeposit(sentence);
	else if (sentence[1] == '2')
		processWithdraw(sentence);
	else if (sentence[1] == '3')
		processTransfer(sentence);
}

void processSuccessReturn(char mSentence[], string s){
	
	int amt = get_amount_sentence(mSentence, 4);
	cout << s << " processed successfully\nThe balance in the account is : " << amt << "\n\n" << endl;
}

void processSuccessTransfer(char mSentence[]){
	
	int amt = get_amount_sentence(mSentence, 4);
	cout << "Transfer processed successfully\nThe balance in the source account is : " << amt << endl;
	amt = get_amount_sentence(mSentence, 24);
	cout << "The balance in the destination account is : " << amt << "\n\n" << endl;
}

void processReturn(char mSentence[]){
	
	if (mSentence[0] == '1'){
		cout << "You are unable to withdraw from your savings account" << endl;
		return;
	}
	else if (mSentence[0] == '2'){
		cout << "The amount requested has exceeded the balance of the account" << endl;
		return;
	}
	else if (mSentence[0] == '3'){
		cout << "Please request withdraw amounts in multiples of 20" << endl;
		return;
	}	
	else if (mSentence[0] == '4'){
		cout << "The amount requested has exceeded the balance of the source account" << endl;
		return;
	}
	
	if (mSentence[1] == '0'){
		processSuccessReturn(mSentence, "Check balance");
	}
	else if (mSentence[1] == '1'){
		processSuccessReturn(mSentence, "Deposit");
	}
	else if (mSentence[1] == '2'){
		processSuccessReturn(mSentence, "Withdraw");
	}
	else if (mSentence[1] == '3'){
		processSuccessTransfer(mSentence);
	}
	
}

int main(void) {

	
   int sock_client;  /* Socket used by client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
   char server_hostname[STRING_SIZE]; /* Server's hostname */
   unsigned short server_port;  /* Port number used by server (remote port) */

   char sentence[STRING_SIZE];  /* send message */
   char modifiedSentence[STRING_SIZE]; /* receive message */
   unsigned int msg_len;  /* length of message */                   
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
  
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
                                    sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
   }
  
   /* user interface */
   
	int choice;

	while (1){
		choice = displayMainMenu(sentence);
		
		if (choice > 0)
			break;
		
		processRequest(sentence);
		msg_len = strlen(sentence) + 1;
		
		/* send message */
   
		bytes_sent = send(sock_client, sentence, msg_len, 0);

		/* get response from server */
		bytes_recd = recv(sock_client, modifiedSentence, STRING_SIZE, 0); 

		printf("\nThe response from server is:\n");
		//printf("%s\n\n", modifiedSentence);
		processReturn(modifiedSentence);
	}
	
   /* close the socket */

   close (sock_client);
}

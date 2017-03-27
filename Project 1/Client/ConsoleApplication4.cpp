// ConsoleApplication4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string>
#include <bitset>
using namespace std;
#define STRING_SIZE 1024

char Int_to_Char_conversion(int number){
	char digits[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	char achar = digits[number];
	return achar; 
}
void Balance_Request(char sentence[], int Account, int Transaction_Type){
	sentence[0] = '0';
	sentence[1] = Int_to_Char_conversion(Transaction_Type);
	sentence[2] = Int_to_Char_conversion(Account);
	sentence[3] = Int_to_Char_conversion(Account);
	sentence[4] = '\o';

	//Just need to send Sentence at this point
}
int Rounded_Amount(int money){
	int Quotient = money / 20; //increments of 20
	return Quotient * 20; //Returns the amount, minus the remainder
}
void Deposit_Request(char sentence[], int Account, int Transaction_Type){
	int money = 0;
	
	sentence[0] = '0';
	sentence[1] = Int_to_Char_conversion(Transaction_Type);
	sentence[2] = Int_to_Char_conversion(Account);
	sentence[3] = Int_to_Char_conversion(Account);
	cout << "How much would you like to deposit. Whole Dollars only. All amounts will be rounded down to the nearest 20" << endl;
	cin >> money;
	money = Rounded_Amount(money);
	std::bitset<20> b(money); //convent number into bit array
	

	

	
}
int _tmain(int argc, _TCHAR* argv[]){
	

	//Char array initialized  
	char sentence[STRING_SIZE];
	 // local variable declaration:
		int Switch_Menu;
		int Account_Type;
		//Do while that will allow client to navigate through options
		do {
			cout << "What would you like to do today?" << endl;
			cin >> Switch_Menu; // User inputs what they want to do via switch statement

			switch (Switch_Menu) {
			case 0: //Check Balance Option
				cout << "Check Balance" << endl;
				cout << "Enter your Account: 0 for Checking, 1 for Savings" << endl;
				cin >> Account_Type;
				Balance_Request(sentence, Account_Type, Switch_Menu); //Function that fills sentence to be sent
				break;
			case 1: //Deposit Option
				cout << "Deposit" << endl;
				cout << "Enter your Account: 0 for Checking, 1 for Savings" << endl;
				cin >> Account_Type;
				Deposit_Request(sentence, Account_Type, Switch_Menu); //Function that fills sentence to be sent
				break;
			case 2:
				cout << "Withdrawl" << endl;
				cout << "Enter your Account: 0 for Checking, 1 for Savings" << endl;
				cin >> Account_Type;
				break;
			case 3:
				cout << "Transfer" << endl;
				break;
			case 4:
				cout << "Closing Program" << endl;
				break;
			default:
				cout << "Invalid Option" << endl;
			}

			cout << "Transactions Completed " << Switch_Menu << endl;
		} while (Switch_Menu != 4);
	return 0;

}


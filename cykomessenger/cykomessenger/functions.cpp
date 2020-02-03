#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <conio.h>
#include <string.h>
//#include "cJSON.h"
#include "Cyko_JSON.h"

#define PORT 12345
#define SA struct sockaddr

void create_account();
void signin();
void usermenu();
void create_channel();
void chat_menu();
void join_channel();
void logout();
void send_message();
void refresh();
void channel_members();
void leave_channel();
void search_members();
void search_messages();


// return
// manual
//step by step search

int client_socket;
char* AuthToken;
int check = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void socket_connect() {  //----------------------------- SOCKET CONNECT ---------------------------//

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("WSAStartup failed with error: %d\n", err);
		return;
	}
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	system("cls");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void welcome_screen() {  //------------------------- WELCOME SCREEN -----------------------------//
	printf("															  					 		CyKoMessenger\n");
	Sleep(1000);
	puts("															  					 		To be comfort");
	Sleep(1000);
	puts("															  					 		 To be smart\n");
	Sleep(1000);
	system("cls");
	printf("\n						   Welcome to CyKoMessenger");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Mainmenu() {  //---------------------------------- MAIN MENU --------------------------------//
	
	Sleep(1000);
	system("cls");
	printf("\nMain Menu\n\n");
	puts("1- Signin Now\n");
	puts("(Not have an account?)\n2- SignUp !!!");
	int respond;
	do {
		scanf("%d", &respond);
	} while (respond > 2 || respond < 1);
	if (respond == 2)create_account();
	if (respond == 1)signin();
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void create_account() {  //----------------------------------- CREATE ACCOUNT -------------------------------//

	system("cls");
	
	//Defining the register info struct

	typedef struct
	{
		char username[100];
		char password[100];
	}client;
	client user;
	puts("Now enter your information :\n");
		puts("Username:");
		scanf("%s", user.username);
		puts("Password:");
		scanf("%s", user.password);

	/*************************** Defining the buffer string *****************************/

	char buffer[100];

	//Making the request string

	sprintf(buffer, "register %s, %s\n", user.username, user.password);

	/************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);


	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/************************************************************************/

	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	char *content = GetObjectItem_ckJSON(out, "content")->valuestring;
	if (!strcmp(type, "Error")) {
		printf("%s", content);
		Sleep(500);
		system("cls");
		Delete_ckJSON(out);
		Mainmenu();
	}
	printf("%s", content);
	Sleep(500);
	Mainmenu();

	//for (;;) {
	//	int c = _getch();
	//	switch (c)
	//	{
	//	case '\r':
	//	case '\n':
	//	case EOF:
	//		_putch('\n');
	//		break;
	//	default:
	//		_putch('*'); //mask
	//		thePassword += char(c);
	//		continue;
	//	}
	//	break;
	//}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void signin() {   //------------------------------------ SIGN-IN ---------------------------------------//

	system("cls");

	typedef struct
	{
		char username[100];
		char password[100];
	}client;
	client user;
	puts("Signin :\n");
	puts("Username:");
	scanf("%s", user.username);
	puts("Password:");
	int counter = 0, ch;
	char *pass = (char *)malloc(40);
	while (TRUE) {
		ch = getch();
		if (ch == 13)break;
		else {
			user.password[counter] = (char)ch;
			counter++;
			printf("*");
		}
	}
	user.password[counter] = '\0';

	/******************************** Defining the buffer string ***********************************/

	char buffer[100];

	/******************************** Making the request string ************************************/

	sprintf(buffer, "login %s, %s\n", user.username, user.password);

	/***********************************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);


	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/***********************************************************************************************/

	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);

	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	char *content = GetObjectItem_ckJSON(out, "content")->valuestring;
	if (!strcmp(type, "Error")) {
		printf("%s", content);
		Sleep(500);
		if ((!strncmp(content, "The user", 8))||(!strncmp(content, "Authenti", 8))) usermenu();
		else {
			system("cls");
			Delete_ckJSON(out);
			Mainmenu();
		}
	}
	else {
		AuthToken = GetObjectItem_ckJSON(out, "content")->valuestring;
		usermenu();
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void usermenu() {   /*-------------------------- USER MENU ------------------------*/

	system("cls");

	if (!check) { printf("\n						         WELCOME "); check = 1; }
	Sleep(500);
	system("cls");

	printf("\n User Menu\n\n1- Create Channel\n2- Join Channel\n3- Logout\n");
	int respond;
	do {
		scanf("%d", &respond);
	} while (respond > 3 || respond < 1);
	if (respond == 1) create_channel();
	else if (respond == 2) join_channel();
	else logout();


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void create_channel() {  //------------------------------- CREATE CHANNEL --------------------------------//

	system("cls");
	char channel_name[100];
	puts("Choose your channel name :");
	scanf("%s", channel_name);

	/******************************** Defining the buffer string ***********************************/

	char buffer[100];

	/******************************** Making the request string ************************************/

	sprintf(buffer, "create channel %s, %s\n", channel_name, AuthToken);

	/***********************************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/*************************************** SENDING REQUEST ***************************************/

	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	char *content = GetObjectItem_ckJSON(out, "content")->valuestring;
	if (!strcmp(type, "Error")) {
		printf("%s", content);
		Sleep(2600);
		system("cls");
		Delete_ckJSON(out);
		usermenu();
	}
	else chat_menu();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void chat_menu() {  /*----------------------- CHAT MENU --------------------------*/

	system("cls");

	printf("\nChat Menu\n1- Send Message\n2- Refresh\n3- Channel Members\n4- Search it!\n5- Leave Channel\n");
		
	int respond;
	do {
		scanf("%d", &respond);
	} while (respond > 5 || respond < 1);
	if (respond == 1) send_message();
	else if (respond == 2) refresh();
	else if (respond == 3) channel_members();
	else if (respond == 4) {
		system("cls");
		puts("1- Members\n2- Messages");
		int pond;
		do {
			scanf("%d", &pond);
		} while (pond > 2 || pond < 1);
		if (pond == 1) search_members();
		else search_messages();
	}
	else leave_channel() ;


	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void join_channel() {  /*----------------------- JOIN CHANNEL --------------------------*/
	system("cls");
	char channel_name[100];
	puts("Channel name :");
	scanf("%s", channel_name);


	/******************************** Defining the buffer string ***********************************/

	char buffer[100];

	/******************************** Making the request string ************************************/

	sprintf(buffer, "join channel %s, %s\n", channel_name, AuthToken);

	/***********************************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/*************************************** SENDING REQUEST ***************************************/

	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	char *content = GetObjectItem_ckJSON(out, "content")->valuestring;
	if (!strcmp(type, "Error")) {
		printf("%s", content);
		Sleep(2600);
		system("cls");
		Delete_ckJSON(out);
		usermenu();
	}
	else chat_menu();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void logout() {  /*-------------------------- LOGOUT -----------------------------*/

	/******************************** Defining the buffer string ***********************************/

	char buffer[100];

	/******************************** Making the request string ************************************/

	sprintf(buffer, "logout %s\n", AuthToken);

	/***********************************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/*************************************** SENDING REQUEST ***************************************/

	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	char *content = GetObjectItem_ckJSON(out, "content")->valuestring;
	if (!strcmp(type, "Error")) {
		printf("%s", content);
		Sleep(2600);
		system("cls");
		Delete_ckJSON(out);
		usermenu();
	}
	else Mainmenu();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void send_message() {  /*----------------------- SEND MESSAGE --------------------------*/

	system("cls");
	char message[100];
	puts("Type your message :");
	scanf("%*c%[^\n]%*c", message);


	/******************************** Defining the buffer string ***********************************/

	char buffer[100];

	/******************************** Making the request string ************************************/

	sprintf(buffer, "send %s, %s\n", message, AuthToken);

	/***********************************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/*************************************** SENDING REQUEST ***************************************/

	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	char *content = GetObjectItem_ckJSON(out, "content")->valuestring;
	if (!strcmp(type, "Error")) {
		printf("%s", content);
		Sleep(2600);
		system("cls");
		Delete_ckJSON(out);
		chat_menu();
	}
	else chat_menu();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void refresh() {  /*----------------------- REFRESH --------------------------*/

	system("cls");
	puts("Messages:");

	/******************************** Defining the buffer string ***********************************/

	char buffer[10240];

	/******************************** Making the request string ************************************/

	sprintf(buffer, "refresh %s\n", AuthToken);

	/***********************************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/*************************************** SENDING REQUEST ***************************************/
	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	ckJSON *cont = GetObjectItem_ckJSON(out, "content");
	int count = GetArraySize_ckJSON(cont);
	for (int i = 0; i < count; i++) {
		ckJSON* mas = GetArrayItem_ckJSON(cont, i);
		char* sender = GetObjectItem_ckJSON(mas, "sender")->valuestring;
		char* ms = GetObjectItem_ckJSON(mas, "content")->valuestring;
		printf("%s: %s\n", sender, ms);
	}
	if (!strcmp(type, "Error")) {
		puts("Oops;\n850 NOT FOUND");
		Sleep(2600);
		system("cls");
		Delete_ckJSON(out);
		chat_menu();
	}
	else {
		printf("Press any key to return ...");
		getchar();
		getchar();
		chat_menu();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void channel_members() {  /*----------------------- CHANNEL MEMBERS --------------------------*/

	system("cls");
	puts("Channel members :");

	/******************************** Defining the buffer string ***********************************/

	char buffer[100];

	/******************************** Making the request string ************************************/

	sprintf(buffer, "channel members %s\n", AuthToken);

	/***********************************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/*************************************** SENDING REQUEST ***************************************/

	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	ckJSON *cont = GetObjectItem_ckJSON(out, "content");
	int count = GetArraySize_ckJSON(cont);
	for (int i = 0; i < count; i++) {
		char* str = GetArrayItem_ckJSON(cont, i)->valuestring;
		printf("%d-%s\n",i+1, str);
	}
	
	if (!strcmp(type, "Error")) {
		puts("Member not found!");
		Sleep(2600);
		system("cls");
		Delete_ckJSON(out);
		chat_menu();
	}
	else { 
		printf("Press any key to return ...");
		getch();
		chat_menu();
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void leave_channel() { /*----------------------- LEAVE CHANNEL --------------------------*/

	/******************************** Defining the buffer string ***********************************/

	char buffer[100];

	/******************************** Making the request string ************************************/

	sprintf(buffer, "leave %s\n", AuthToken);

	/***********************************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/*************************************** SENDING REQUEST ***************************************/

	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	char *content = GetObjectItem_ckJSON(out, "content")->valuestring;
	if (!strcmp(type, "Error")) {
		printf("%s", content);
		Sleep(2600);
		system("cls");
		Delete_ckJSON(out);
		chat_menu();
	}
	else usermenu();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void search_members() {

	char name[100];
	puts("Type the username you wish to find :");
	scanf("%s", name);

	/******************************** Defining the buffer string ***********************************/

	char buffer[100];

	/******************************** Making the request string ************************************/

	sprintf(buffer, "search member %s,%s\n",name, AuthToken);

	/***********************************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/*************************************** SENDING REQUEST ***************************************/

	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	char *content = GetObjectItem_ckJSON(out, "content")->valuestring;
	if (!strcmp(type, "Error")) {
		printf("%s", content);
		Sleep(2600);
		system("cls");
		Delete_ckJSON(out);
		chat_menu();
	}
	else {
		if (strcmp(content, "true") == 0) printf("%s is now in the channel!",name);
		else puts("No member was found with this username :(");
		Sleep(2600);
		system("cls");
		Delete_ckJSON(out);
		chat_menu();
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void search_messages() {

	char message[100];
	puts("Type the message you wish to find :");
	scanf("%*c%[^\n]%*c", message);

	/******************************** Defining the buffer string ***********************************/

	char buffer[10000];

	/******************************** Making the request string ************************************/

	sprintf(buffer, "search message %s,%s\n", message, AuthToken);

	/***********************************************************************************************/

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	connect(client_socket, (SA*)&servaddr, sizeof(servaddr));

	/*************************************** SENDING REQUEST ***************************************/
	send(client_socket, buffer, sizeof(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	ckJSON* out = Parse_ckJSON(buffer);
	char *type = GetObjectItem_ckJSON(out, "type")->valuestring;
	ckJSON *cont = GetObjectItem_ckJSON(out, "message");
	int count = GetArraySize_ckJSON(cont);
	if (!count) puts("No message was found!");
	else puts("\n----------------------------------------------------------");
	for (int i = 0; i < count; i++) {
		char* mas = GetArrayItem_ckJSON(cont, i)->valuestring;
		printf("%s\n", mas);
	}
	if (!strcmp(type, "Error")) {
		printf("%s", cont);
		Sleep(2600);
		system("cls");
		Delete_ckJSON(out);
		chat_menu();
	}
	else {
		puts("----------------------------------------------------------");
		printf("\nPress any key to return ...");
		getchar(); 
		chat_menu();
	}
}
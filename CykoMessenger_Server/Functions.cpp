#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include <io.h> 
#include <conio.h>
#include "cJSON.h"
#include "cJSON.c"
#include <string.h>
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

void recieve();
void socket_connect();
void order_finder(char* buffer);
void create_account();
void signin();
char* token_generator();
void create_channel();
//void join_channel();
//void logout();
//void send_message();
//void refresh();
//void channel_members();
//void leave_channel();


int server_socket, client_socket;
struct sockaddr_in server, client;


typedef struct use
{
	char username[100];
	char password[100];
	char token[100];
}USER;
USER user;

typedef struct chann
{
	char name[100];
	char message[100000];
}CHANNEL;
USER channel;


void socket_connect()
{

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		// Tell the user that we could not find a usable Winsock DLL.
		printf("WSAStartup failed with error: %d\n", err);
		return;
	}

	// Create and verify socket
	server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
		wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
	else
		printf("Socket successfully created...\n");

	// Assign IP and port
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	// Bind newly created socket to given IP and verify
	if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
	{
		printf("Socket binding failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully bound...\n");

	// Now server is ready to listen and verify
	if ((listen(server_socket, 5)) != 0)
	{
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");

	int flag = 0;
	while (1)
	{
		// Accept the data packet from client and verify
		int len = sizeof(client);
		client_socket = accept(server_socket, (SA *)&client, &len);
		if (client_socket < 0)
		{
			printf("Server accceptance failed...\n");
			exit(0);
		}
		else
			if (!flag) {
				flag = 1;
				printf("Server acccepted the client..\n");
			}

		// Function for chatting between client and server
		recieve();

	}

	// Close the socket
	closesocket(server_socket);
}



// Function designed for chat between client and server.
void recieve()
{
	char buffer[MAX];
	int n;
	memset(buffer, 0, sizeof(buffer));

	// Read the message from client and copy it to buffer
	recv(client_socket, buffer, sizeof(buffer), 0);

	order_finder(buffer);

	// Print buffer which contains the client message
	//printf("From client: %s\t To client : ", buffer);
	//memset(buffer, 0, sizeof(buffer));
	//n = 0;
	//// Copy server message to the buffer
	//while ((buffer[n++] = getchar()) != '\n')
	//	;

	//// Send the buffer to client
	//send(client_socket, buffer, sizeof(buffer), 0);

	// If the message starts with "exit" then server exits and chat ends
	if (strncmp("exit", buffer, 4) == 0)
	{
		printf("Server stopping...\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void order_finder(char* buffer) {
	if (strncmp(buffer, "register", 8) == 0) {
		sscanf(buffer, "%*s %[^',']%*c%s", user.username, user.password);
		create_account();
	}
	if (strncmp(buffer, "login", 5) == 0) {
		sscanf(buffer, "%*s %[^',']%*c%s", user.username, user.password);
		signin();
	}
	/*if (strncmp(buffer, "create channel", 14) == 0) return 3;
	if (strncmp(buffer, "join channel", 12) == 0)return 4;
	if (strncmp(buffer, "send", 4) == 0)return 5;
	if (strncmp(buffer, "Refresh", 7) == 0)return 6;
	if (strncmp(buffer, "channel members", 15) == 0)return 7;
	if (strncmp(buffer, "leave", 5) == 0)return 8;
	if (strncmp(buffer, "logout", 6) == 0)return 9;*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void create_account() {  //----------------------------------- CREATE ACCOUNT -------------------------------//

	char buffer[MAX];
	FILE* userfile;
	char filename[100];
	sprintf(filename, "./Resources/Users/%s.cyko", user.username);
	if (access(filename, 0) != -1) {	// file exists
		sprintf(buffer, "{\"type\": \"Error\", \"content\": \"User already exists!\"}");
	}
	else {	// file doesn't exist
		sprintf(buffer, "{\"type\": \"Successful\", \"content\": \"\"}");
		userfile = fopen(filename, "w");
	}
	// Saving in file
	fprintf(userfile, "{\"username\":\"%s\",\"password\":\"%s\"}", user.username, user.password);
	fclose(userfile);

	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void signin() {

	char buffer[MAX];
	char filename[100];
	sprintf(filename, "./Resources/Users/%s.cyko", user.username);
	if (access(filename, 0) != -1) {    // file exists
		FILE* userfile;
		userfile = fopen(filename, "r");
		char js[100];
		fgets(js, 100, userfile);
		cJSON* out = cJSON_Parse(js);
		char *password = cJSON_GetObjectItem(out, "password")->valuestring;
		if (strcmp(password, user.password) == 0)
			sprintf(buffer, "{\"type\": \"AuthToken\", \"content\": \"%s\"}", token_generator());
		else
			sprintf(buffer, "{\"type\": \"Error\", \"content\": \"Password is not correct!\"}");
	}

	else 	// file doesn't exist
		sprintf(buffer, "{\"type\": \"Error\", \"content\": \"User doesnt exists!\"}");
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* token_generator() {
	srand(time(NULL));
	char *string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int n = 16;
	char token[17];
	for (int i = 0; i < n; i++)
		token[i] = string[rand() % 62];
	token[16] = '\0';
	return token;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void create_channel() {

	char buffer[MAX];
	FILE* channelfile;
	char filename[100];
	sprintf(filename, "./Resources/Channels/%s.cyko",channel.namme);
	if (access(filename, 0) != -1) {	// file exists
		sprintf(buffer, "{\"type\": \"Error\", \"content\": \"User already exists!\"}");
	}
	else {	// file doesn't exist
		sprintf(buffer, "{\"type\": \"Successful\", \"content\": \"\"}");
		userfile = fopen(filename, "w");
	}
	// Saving in file
	fprintf(userfile, "{\"username\":\"%s\",\"password\":\"%s\"}", user.username, user.password);
	fclose(userfile);

	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}
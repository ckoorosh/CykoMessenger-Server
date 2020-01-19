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
void join_channel();
void send_message();
//void logout();
void refresh();
//void channel_members();
//void leave_channel();


int server_socket, client_socket;
int channel_id=0, user_id=0;
char token[100];
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
	char message[100];
	int last_message;
	int id;
}CHANNEL;
CHANNEL channel;





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
	else if (strncmp(buffer, "login", 5) == 0) {
		sscanf(buffer, "%*s %[^',']%*c%s", user.username, user.password);
		signin();
	}
	else if (strncmp(buffer, "create channel", 14) == 0) {
		sscanf(buffer, "%*s%*s %[^',']%*c%s", channel.name, token);
		create_channel();
	}
	else if (strncmp(buffer, "join channel", 12) == 0) {
		sscanf(buffer, "%*s%*s %[^',']%*c%s", channel.name, token);
		join_channel();
	}
	else if (strncmp(buffer, "send", 4) == 0) {
		sscanf(buffer, "%*s %[^',']%*c%s", channel.message, token);
		send_message();
	}
	if (strncmp(buffer, "Refresh", 7) == 0) {
		sscanf(buffer, "%*s %s",  token);
	}
	/*if (strncmp(buffer, "channel members", 15) == 0)return 7;
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
		// Saving in file
		fprintf(userfile, "{\"username\":\"%s\",\"password\":\"%s\"}", user.username, user.password);
		fclose(userfile);
		user_id++;
	}
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
		fgets(js,100,userfile);
		cJSON* out = cJSON_Parse(js);
		char *password = cJSON_GetObjectItem(out, "password")->valuestring;
		if (strcmp(password, user.password) == 0) {
			strcpy(user.token , token_generator());
			sprintf(buffer, "{\"type\": \"AuthToken\", \"content\": \"%s\"}",user.token );
		}
		else
			sprintf(buffer, "{\"type\": \"Error\", \"content\": \"Password is not correct!\"}");
	}

	else 	// file doesn't exist
		sprintf(buffer, "{\"type\": \"Error\", \"content\": \"User does not exist!\"}");
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* token_generator() {
	srand(time(NULL));
	char *string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int n = 16;
	char tkn[17];
	for (int i = 0; i < n; i++)
		tkn[i]=string[rand() % 62 ];
	tkn[16] = '\0';
	return tkn;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void create_channel() {

	char buffer[MAX];
	if (strcmp(token, user.token) != 0) sprintf(buffer, "{\"type\": \"Error\", \"content\": \"Authentication failed!\"}");
	else {
		char filename[100];
		sprintf(filename, "./Resources/Channels/%s.cyko", channel.name);
		
		if (access(filename, 0) != -1) {    // file exists		
			sprintf(buffer, "{\"type\": \"Error\", \"content\": \"Channel already exists!\"}");
		}

		else { 	// file doesn't exist

				/************************* Creating JSON ***************************/
			cJSON *add, *messages, *message;

			add = cJSON_CreateObject();
			messages = cJSON_CreateArray();

			cJSON_AddItemToObject(add, "messages", messages);

			char buff[100];
			cJSON_AddItemToArray(messages, message = cJSON_CreateObject());
			cJSON_AddItemToObject(message, "sender", cJSON_CreateString("server"));
			sprintf(buff, "%s created channel", user.username);
			cJSON_AddItemToObject(message, "content", cJSON_CreateString(buff));

			cJSON_AddItemToObject(add, "name", cJSON_CreateString(channel.name));
			cJSON_Delete(add);

			char* addjs;
			FILE* channelfile;
			channelfile = fopen(filename, "w");
			addjs = cJSON_PrintUnformatted(add);
			fprintf(channelfile, "%s", addjs);
			fclose(channelfile);

			sprintf(buffer, "{\"type\": \"Successful\", \"content\": \"\"}");
		}
		// Send the buffer to client
		send(client_socket, buffer, sizeof(buffer), 0);

	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void join_channel() {

	channel.last_message = -1;
	char buffer[MAX];
	if (strcmp(token, user.token) != 0) 	sprintf(buffer, "{\"type\": \"Error\", \"content\": \"Authentication failed!\"}");
	else {
		FILE* channelfile;
		char filename[100];
		sprintf(filename, "./Resources/Channels/%s.cyko", channel.name);
		if (access(filename, 0) != -1) {	// file exists
			sprintf(buffer, "{\"type\": \"Successful\", \"content\": \"\"}");

			/************************* Updting the messages ****************************/
			channelfile = fopen(filename, "r");
			char js[10000];
			fgets(js, 10000, channelfile);
			fclose(channelfile);

			printf("%s", js);

			cJSON *msg_array, *item;
			cJSON *messages = cJSON_Parse(js);
			msg_array = cJSON_GetObjectItem(messages, "messages");

			// Create a new array item and add sender and message
			item = cJSON_CreateObject();
			cJSON_AddItemToObject(item, "sender", cJSON_CreateString("server"));
			char buf[100];
			sprintf(buf, "%s joined channel %s", user.username, channel.name);
			cJSON_AddItemToObject(item, "message", cJSON_CreateString(buf));

			// insert the new message into the existing array
			cJSON_AddItemToArray(msg_array, item);
			
			channelfile = fopen(filename, "w");
			fprintf(channelfile, cJSON_PrintUnformatted(messages));

			cJSON_Delete(messages);

			// Closing the file
			fclose(channelfile);
		}
		else {	// file doesn't exist
			sprintf(buffer, "{\"type\": \"Error\", \"content\": \"Channel does not exist!\"}");
		}
	}
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void send_message() {

	char buffer[MAX];
	if (strcmp(token, user.token) != 0) 	sprintf(buffer, "{\"type\": \"Error\", \"content\": \"Authentication failed!\"}");
	else {
		sprintf(buffer, "{\"type\": \"Successful\", \"content\": \"\"}");

		/******************* Extracting messages from the file ********************/

		FILE* channelfile;
		char filename[100];
		sprintf(filename, "./Resources/Channels/%s.cyko", channel.name);

		channelfile = fopen(filename, "r");
		char js[10000];
		fgets(js, 10000, channelfile);
		printf("%s", js);

		/************************* Updting the messages ****************************/

		cJSON *msg_array, *item;
		cJSON *messages = cJSON_Parse(js);
		msg_array = cJSON_GetObjectItem(messages, "messages");

		// Create a new array item and add sender and message
		item = cJSON_CreateObject();
		cJSON_AddItemToObject(item, "sender", cJSON_CreateString(user.username));
		cJSON_AddItemToObject(item, "message", cJSON_CreateString(channel.message));

		// insert the new message into the existing array
		cJSON_AddItemToArray(msg_array, item);

		fprintf(channelfile, cJSON_PrintUnformatted(messages));

		cJSON_Delete(messages);
	}

	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void refresh() {
	char* allmsg="",*mssg="";
	char filename[100];
	sprintf(filename, "./Resources/Channels/%s.cyko", channel.name);

	/***************** Extracting messages from the file ********************/

	FILE* channelfile;
	sprintf(filename, "./Resources/Channels/%s.cyko", channel.name);
	channelfile = fopen(filename, "w");
	char js[10000];
	fgets(js, 10000, channelfile);
	printf("%s", js);
	
	/***************************** Refreshing ... ****************************/

	cJSON* out = cJSON_Parse(js);
	allmsg = cJSON_GetObjectItem(out, "messages")->valuestring;
	printf("%s", allmsg);
	for (int i = channel.last_message + 1; allmsg[i]; i++)
		mssg[i-(channel.last_message + 1)]=allmsg[i];
	sprintf(allmsg, "\"type\": \"List\",\"content\":%s", allmsg);
	channel.last_message = strlen(allmsg)-1;
	printf("%s", mssg);
	// Send the buffer to client
	send(client_socket,mssg, sizeof(mssg), 0);
}
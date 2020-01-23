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
void refresh();
void channel_members();
void leave_channel();
void logout();



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
	if (strncmp(buffer, "refresh", 7) == 0) {
		sscanf(buffer, "%*s %s",  token);
		refresh();
	}
	if (strncmp(buffer, "channel members", 15) == 0) {
		sscanf(buffer, "%*s%*s%s", token);
		channel_members();
	}
	if (strncmp(buffer, "leave", 5) == 0) {
		sscanf(buffer, "%*s %s", token);
		leave_channel();
	}
	if (strncmp(buffer, "logout", 6) == 0) {
		sscanf(buffer, "%*s %s", token);
		logout();
	}
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

			FILE* channelfile;
			channelfile = fopen(filename, "w");
			char* addjs;
			addjs = cJSON_PrintUnformatted(add);
			fprintf(channelfile, "%s", addjs);
			fclose(channelfile);
			cJSON_Delete(add);

			sprintf(buffer, "{\"type\": \"Successful\", \"content\": \"\"}");
		}
		// Send the buffer to client
		send(client_socket, buffer, sizeof(buffer), 0);

	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void join_channel() {

	channel.last_message = 0;
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

			cJSON *msg_array, *item;
			cJSON *messages = cJSON_Parse(js);
			msg_array = cJSON_GetObjectItem(messages, "messages");

			// Create a new array item and add sender and message
			item = cJSON_CreateObject();
			cJSON_AddItemToObject(item, "sender", cJSON_CreateString("server"));
			char buf[100];
			sprintf(buf, "%s joined channel %s", user.username, channel.name);
			cJSON_AddItemToObject(item, "content", cJSON_CreateString(buf));

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
		fclose(channelfile);

		/************************* Updting the messages ****************************/

		cJSON *msg_array, *item;
		cJSON *messages = cJSON_Parse(js);
		msg_array = cJSON_GetObjectItem(messages, "messages");

		// Create a new array item and add sender and message
		item = cJSON_CreateObject();
		cJSON_AddItemToObject(item, "sender", cJSON_CreateString(user.username));
		cJSON_AddItemToObject(item, "content", cJSON_CreateString(channel.message));

		// insert the new message into the existing array
		cJSON_AddItemToArray(msg_array, item);

		channelfile = fopen(filename, "w");
		fprintf(channelfile, cJSON_PrintUnformatted(messages));
		fclose(channelfile);
		cJSON_Delete(messages);
	}

	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void refresh() {
	char mssg[10000]="",end[10000]="";
	char filename[100];

	if (strcmp(token, user.token) != 0) 	sprintf(end, "{\"type\": \"Error\", \"content\": \"Authentication failed!\"}");
	else {

		/***************** Extracting messages from the file ********************/
		FILE* channelfile;
		sprintf(filename, "./Resources/Channels/%s.cyko", channel.name);
		channelfile = fopen(filename, "r");
		char js[10000];
		fgets(js, 10000, channelfile);
		fclose(channelfile);

		/***************************** Refreshing ... ****************************/
		cJSON* out = cJSON_Parse(js);
		cJSON *cont = cJSON_GetObjectItem(out, "messages");
		char *allmsg = cJSON_PrintUnformatted(cont);
		for (int i = channel.last_message + 1; allmsg[i+1]; i++)
			mssg[i - (channel.last_message + 1)] = allmsg[i];
		sprintf(end, "{\"type\": \"List\",\"content\":[%s]}", mssg);
		channel.last_message = strlen(allmsg) - 1;
	}
	
	// Send the buffer to client
	send(client_socket,end, sizeof(end), 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void channel_members() {

	char buffer[MAX];
	if (strcmp(token, user.token) != 0) 	sprintf(buffer, "{\"type\": \"Error\", \"content\": \"Authentication failed!\"}");
	else {
		
		cJSON *add, *content;

		add = cJSON_CreateObject();
		content = cJSON_CreateArray();

		cJSON_AddItemToObject(add, "type", cJSON_CreateString("list"));
		cJSON_AddItemToObject(add, "content", content);
		cJSON_AddItemToArray(content, cJSON_CreateString(user.username));
		strcpy(buffer,cJSON_PrintUnformatted(add));
		printf("%s", buffer);
		cJSON_Delete(add);
	}
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void leave_channel() {

	char buffer[MAX];
	if (strcmp(token, user.token) != 0) 	sprintf(buffer, "{\"type\": \"Error\", \"content\": \"Authentication failed!\"}");
	else {
		sprintf(buffer, "{\"type\": \"Successful\", \"content\": \"\"}");
	}
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void logout() {

	char buffer[MAX];
	if (strcmp(token, user.token) != 0) 	sprintf(buffer, "{\"type\": \"Error\", \"content\": \"Authentication failed!\"}");
	else {
		sprintf(buffer, "{\"type\": \"Successful\", \"content\": \"\"}");
	}
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

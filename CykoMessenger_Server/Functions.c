#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include <io.h> 
#include <time.h>
#include <conio.h>
//#include "cJSON.h"
//#include "cJSON.c"
#include "Cyko_JSON.h"
#include "Cyko_JSON.c"
#include <string.h>
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

void recieve();
void socket_connect();
void order_finder(char* buffer);
void create_account();
void signin();
int online(char*);
char* token_generator();
int id_finder(char*);
void create_channel();
int channel_finder(char*);
void join_channel();
void send_message();
void refresh();
void channel_members();
void leave_channel();
void logout();
void search_members();
void search_message();



int server_socket, client_socket;
int channel_id=0, user_id=0;
char username[100],password[100],token[100],channelname[100],message[100];
struct sockaddr_in server, client;


// The User structure
typedef struct use
{
	char username[100];
	char password[100];
	char token[100];
	int channel_id;
}USER;
USER user[100];

// The Channel structure
typedef struct chann
{
	char name[100];
	int last_message;
	int user_id[100];
	int users;
}CHANNEL;
CHANNEL channel[100];





void recieve()   /************************** Recieve *****************************/
{
	// Function designed for chatting between client and server.

	char buffer[MAX];

	memset(buffer, 0, sizeof(buffer));

	// Read the message from client and copy it to buffer
	recv(client_socket, buffer, sizeof(buffer), 0);

	// Print buffer which contains the client message
	printf("\rThe client asks : %s", buffer);

	// Function to find what is the client's request
	order_finder(buffer);

	memset(buffer, 0, sizeof(buffer));


	// If the message starts with "exit" then server exits and chat ends
	if (strncmp("exit", buffer, 4) == 0)
	{
		printf("Server stopping...\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void order_finder(char* buffer) {  /************************** Order Finder *****************************/

	if (strncmp(buffer, "register", 8) == 0) {
		sscanf(buffer, "%*s %[^',']%*c%s", username, password);
		create_account();
	}
	else if (strncmp(buffer, "login", 5) == 0) {
		sscanf(buffer, "%*s %[^',']%*c%s", username, password);
		signin();
	}
	else if (strncmp(buffer, "create channel", 14) == 0) {
		sscanf(buffer, "%*s%*s %[^',']%*c%s", channelname, token);
		create_channel();
	}
	else if (strncmp(buffer, "join channel", 12) == 0) {
		sscanf(buffer, "%*s%*s %[^',']%*c%s", channelname, token);
		join_channel();
	}
	else if (strncmp(buffer, "send", 4) == 0) {
		sscanf(buffer, "%*s %[^',']%*c%s", message, token);
		send_message();
	}
	else if (strncmp(buffer, "refresh", 7) == 0) {
		sscanf(buffer, "%*s %s",  token);
		refresh();
	}
	else if (strncmp(buffer, "channel members", 15) == 0) {
		sscanf(buffer, "%*s%*s%s", token);
		channel_members();
	}
	else if (strncmp(buffer, "leave", 5) == 0) {
		sscanf(buffer, "%*s %s", token);
		leave_channel();
	}
	else if (strncmp(buffer, "logout", 6) == 0) {
		sscanf(buffer, "%*s %s", token);
		logout();
	}
	else if (strncmp(buffer, "search mem", 10) == 0) {
		sscanf(buffer, "%*s %*s %[^',']%*c%s", username, token);
		search_members();
	}
	else if (strncmp(buffer, "search mes", 10) == 0) {
		sscanf(buffer, "%*s %*s %[^',']%*c%s", message, token);
		search_message();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void create_account() {  //----------------------------------- CREATE ACCOUNT -------------------------------//
	
	char buffer[MAX];
	FILE* userfile;
	char filename[100];
	sprintf(filename, "./Resources/Users/%s.cyko", username);
	if (access(filename, 0) != -1) {	// file exists
		sprintf(buffer, "{\"type\":\"Error\",\"content\":\"User already exists!\"}");
	}
	else {	// file doesn't exist
		sprintf(buffer, "{\"type\":\"Successful\",\"content\":\"\"}");
		userfile = fopen(filename, "w");
		// Saving in file
		fprintf(userfile, "{\"username\":\"%s\",\"password\":\"%s\"}", username, password);
		fclose(userfile);

	}

	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void signin() {   /************************** Sign-In *****************************/

	char buffer[MAX];
	char filename[100];
	sprintf(filename, "./Resources/Users/%s.cyko", username);
	if(online(username)) sprintf(buffer, "{\"type\":\"Error\",\"content\":\"User has already logged in !\"}");
	else if (access(filename, 0) != -1) {    // file exists
		FILE* userfile;
		userfile = fopen(filename, "r");
		char js[100];
		fgets(js,100,userfile);
		ckJSON* out = Parse_ckJSON(js);
		char *pass = GetObjectItem_ckJSON(out, "password")->valuestring;
		if (strcmp(pass, password) == 0) {
			strcpy(user[user_id].token , token_generator());
			sprintf(buffer, "{\"type\":\"AuthToken\",\"content\":\"%s\"}", user[user_id].token);
			user[user_id].channel_id = -1;
			strcpy(user[user_id].username, username);
			user_id++;
		}
		else
			sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Password is not correct!\"}");
	}

	else 	// file doesn't exist
		sprintf(buffer, "{\"type\":\"Error\",\"content\":\"User does not exist!\"}");
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* token_generator() {	 /************************** Token Generator *****************************/

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

int id_finder(char* token) {
	for (int i = 0; i < user_id; i++) {
		if (strcmp(token, user[i].token) == 0) return i;
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void create_channel() {	 /************************** Create Acount *****************************/

	char buffer[MAX];
	if (id_finder(token) == -1) sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Authentication failed!\"}");
	else {
		char filename[100];
		sprintf(filename, "./Resources/Channels/%s.cyko", channelname);
		
		if (access(filename, 0) != -1) {    // file exists		
			sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Channel already exists!\"}");
		}

		else { 	// file doesn't exist

				/************************* Creating JSON ***************************/
			//cJSON *add, *messages, *message;
			ckJSON *add, *messages, *message;

			/*add = cJSON_CreateObject();
			messages = cJSON_CreateArray();*/
			add = CreateObject_ckJSON();
			messages = CreateArray_ckJSON();

			//cJSON_AddItemToObject(add, "messages", messages);
			AddItemToObject_ckJSON(add, "messages", messages);

			char buff[100];
			/*cJSON_AddItemToArray(messages, message = cJSON_CreateObject());
			cJSON_AddItemToObject(message, "sender", cJSON_CreateString("server"));*/
			AddItemToArray_ckJSON(messages, message = CreateObject_ckJSON());
			AddItemToObject_ckJSON(message, "sender", CreateString_ckJSON("server"));
			sprintf(buff, "%s created channel", user[id_finder(token)].username);
			//cJSON_AddItemToObject(message, "content", cJSON_CreateString(buff));
			AddItemToObject_ckJSON(message, "content", CreateString_ckJSON(buff));

			//cJSON_AddItemToObject(add, "name", cJSON_CreateString(channelname));
			AddItemToObject_ckJSON(add, "name", CreateString_ckJSON(channelname));

			FILE* channelfile;
			channelfile = fopen(filename, "w");
			char* addjs;
			//addjs = cJSON_PrintUnformatted(add);
			addjs = PrintUnformatted_ckJSON(add);
			fprintf(channelfile, "%s", addjs);
			fclose(channelfile);
			//cJSON_Delete(add);
			Delete_ckJSON(add);

			sprintf(buffer, "{\"type\":\"Successful\",\"content\":\"\"}");

			strcpy(channel[channel_id].name, channelname);
			channel[channel_id++].users = 0;

			// Assining channel's last read message to 0 and adding the user's id to channel's online users;
			channel[channel_finder(channelname)].last_message = 0;
			channel[channel_finder(channelname)].user_id[channel[channel_finder(channelname)].users++] = id_finder(token);
			user[id_finder(token)].channel_id = channel_finder(channelname);
		}
		
	}
	// Send the buffer to client
		send(client_socket, buffer, sizeof(buffer), 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int channel_finder(char* name) {
	for (int i = 0; i < channel_id; i++) {
		if (strcmp(name, channel[i].name) == 0) return i;
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void join_channel() {	 /************************** Join Channel *****************************/

	char buffer[MAX];
	if (id_finder(token) == -1) 	sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Authentication failed!\"}");
	else {
		FILE* channelfile;
		char filename[100];
		sprintf(filename, "./Resources/Channels/%s.cyko", channelname);
		if (access(filename, 0) != -1) {	// file exists
			sprintf(buffer, "{\"type\":\"Successful\",\"content\":\"\"}");

			/************************* Updting the messages ****************************/
			channelfile = fopen(filename, "r");
			char js[10000];
			fgets(js, 10000, channelfile);
			fclose(channelfile);

			/*cJSON *msg_array, *item;
			cJSON *messages = cJSON_Parse(js);
			msg_array = cJSON_GetObjectItem(messages, "messages");*/
			ckJSON *msg_array, *item;
			ckJSON *messages = Parse_ckJSON(js);
			msg_array = GetObjectItem_ckJSON(messages, "messages");

			// Create a new array item and add sender and message
			/*item = cJSON_CreateObject();
			cJSON_AddItemToObject(item, "sender", cJSON_CreateString("server"));*/
			item = CreateObject_ckJSON();
			AddItemToObject_ckJSON(item, "sender", CreateString_ckJSON("server"));
			char buf[100];
			sprintf(buf, "%s joined channel %s", user[id_finder(token)].username, channelname);
			//cJSON_AddItemToObject(item, "content", cJSON_CreateString(buf));
			AddItemToObject_ckJSON(item, "content", CreateString_ckJSON(buf));

			// insert the new message into the existing array
			//cJSON_AddItemToArray(msg_array, item);
			AddItemToArray_ckJSON(msg_array, item);
			channelfile = fopen(filename, "w");
			//fprintf(channelfile, cJSON_PrintUnformatted(messages));
			fprintf(channelfile, PrintUnformatted_ckJSON(messages));

			//cJSON_Delete(messages);
			Delete_ckJSON(messages);

			// Closing the file
			fclose(channelfile);
			
			strcpy(channel[channel_id].name, channelname);
			channel[channel_id++].users = 0;

			// Assining channel's last read message to 0 and adding the user's id to channel's online users;
			channel[channel_finder(channelname)].last_message = 0;
			channel[channel_finder(channelname)].user_id[channel[channel_finder(channelname)].users++] = id_finder(token);
			user[id_finder(token)].channel_id = channel_finder(channelname);
		
		}
		else {	// file doesn't exist
			sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Channel does not exist!\"}");
		}
	}
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void send_message() {	 /************************** Send Message *****************************/

	printf("%d", user[id_finder(token)].channel_id);
	char buffer[MAX];
	if (id_finder(token) == -1)	sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Authentication failed!\"}");
	else if(user[id_finder(token)].channel_id==-1) sprintf(buffer, "{\"type\":\"Error\",\"content\":\"User is not online right now!\"}");
	else {
		sprintf(buffer, "{\"type\":\"Successful\",\"content\":\"\"}");

		/******************* Extracting messages from the file ********************/

		FILE* channelfile;
		char filename[100];
		sprintf(filename, "./Resources/Channels/%s.cyko", channel[user[id_finder(token)].channel_id].name);

		channelfile = fopen(filename, "r");
		char js[10000];
		fgets(js, 10000, channelfile);
		fclose(channelfile);

		/************************* Updting the messages ****************************/
		/*cJSON *msg_array, *item;
		cJSON *messages = cJSON_Parse(js);
		msg_array = cJSON_GetObjectItem(messages, "messages");*/
		ckJSON *msg_array, *item;
		ckJSON *messages = Parse_ckJSON(js);
		msg_array = GetObjectItem_ckJSON(messages, "messages");
		// Create a new array item and add sender and message
		item = CreateObject_ckJSON();
		AddItemToObject_ckJSON(item, "sender", CreateString_ckJSON(user[id_finder(token)].username));
		AddItemToObject_ckJSON(item, "content", CreateString_ckJSON(message));
		// insert the new message into the existing array
		AddItemToArray_ckJSON(msg_array, item);
		channelfile = fopen(filename, "w");
		fprintf(channelfile, PrintUnformatted_ckJSON(messages));
		fclose(channelfile);
		Delete_ckJSON(messages);
	}

	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void refresh() {	 /************************** Rrefresh *****************************/
	char mssg[10000]="",end[10000]="";
	char filename[100];
	if (id_finder(token) == -1)	sprintf(end, "{\"type\":\"Error\",\"content\":\"Authentication failed!\"}");
	else if (user[id_finder(token)].channel_id == -1) sprintf(end, "{\"type\":\"Error\",\"content\":\"User is not online right now!\"}");
	else {

		/***************** Extracting messages from the file ********************/
		FILE* channelfile;
		sprintf(filename, "./Resources/Channels/%s.cyko", channel[user[id_finder(token)].channel_id].name);
		channelfile = fopen(filename, "r");
		char js[10000];
		fgets(js, 10000, channelfile);
		fclose(channelfile);

		/***************************** Refreshing ... ****************************/
		ckJSON* out = Parse_ckJSON(js);
		ckJSON *cont = GetObjectItem_ckJSON(out, "messages");
		char *allmsg = PrintUnformatted_ckJSON(cont);

		for (int i = channel[user[id_finder(token)].channel_id].last_message + 1; allmsg[i+1]; i++)
			mssg[i - (channel[user[id_finder(token)].channel_id].last_message + 1)] = allmsg[i];
		sprintf(end, "{\"type\":\"List\",\"content\":[%s]}", mssg);
		channel[user[id_finder(token)].channel_id].last_message = strlen(allmsg) - 1;
	}
	
	// Send the buffer to client
	send(client_socket,end, sizeof(end), 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void channel_members() {	 /************************** Channel Members *****************************/

	char buffer[MAX];
	if (id_finder(token) == -1)	sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Authentication failed!\"}");
	else if (user[id_finder(token)].channel_id == -1) sprintf(buffer, "{\"type\":\"Error\",\"content\":\"User is not online right now!\"}");
	else {
		
		ckJSON *add, *content;

		add = CreateObject_ckJSON();
		content = CreateArray_ckJSON();

		AddItemToObject_ckJSON(add, "type", CreateString_ckJSON("list"));
		AddItemToObject_ckJSON(add, "content", content);
		for (int i = 0; i < channel[user[id_finder(token)].channel_id].users;i++)
			AddItemToArray_ckJSON(content, CreateString_ckJSON(user[channel[user[id_finder(token)].channel_id].user_id[i]].username));
		strcpy(buffer,PrintUnformatted_ckJSON(add));
		Delete_ckJSON(add);
	}
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void leave_channel() {	 /************************** Leave channel *****************************/

	char buffer[MAX];
	if (id_finder(token) == -1) 	sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Authentication failed!\"}");
	else if (user[id_finder(token)].channel_id == -1) sprintf(buffer, "{\"type\": \"Error\", \"content\": \"User is not online right now!\"}");
	else {
		FILE* channelfile;
		char filename[100];
		sprintf(filename, "./Resources/Channels/%s.cyko", channelname);
		if (access(filename, 0) != -1) {	// file exists

			/************************* Updting the messages ****************************/
			channelfile = fopen(filename, "r");
			char js[10000];
			fgets(js, 10000, channelfile);
			fclose(channelfile);

			/*cJSON *msg_array, *item;
			cJSON *messages = cJSON_Parse(js);
			msg_array = cJSON_GetObjectItem(messages, "messages");*/
			ckJSON *msg_array, *item;
			ckJSON *messages = Parse_ckJSON(js);
			msg_array = GetObjectItem_ckJSON(messages, "messages");

			// Create a new array item and add sender and message
			/*item = cJSON_CreateObject();
			cJSON_AddItemToObject(item, "sender", cJSON_CreateString("server"));*/
			item = CreateObject_ckJSON();
			AddItemToObject_ckJSON(item, "sender", CreateString_ckJSON("server"));
			char buf[100];
			sprintf(buf, "%s left channel %s", user[id_finder(token)].username, channelname);
			//cJSON_AddItemToObject(item, "content", cJSON_CreateString(buf));
			AddItemToObject_ckJSON(item, "content", CreateString_ckJSON(buf));

			// insert the new message into the existing array
			//cJSON_AddItemToArray(msg_array, item);
			AddItemToArray_ckJSON(msg_array, item);
			channelfile = fopen(filename, "w");
			//fprintf(channelfile, cJSON_PrintUnformatted(messages));
			fprintf(channelfile, PrintUnformatted_ckJSON(messages));

			//cJSON_Delete(messages);
			Delete_ckJSON(messages);

			// Closing the file
			fclose(channelfile);


			int index;
			for (int i = 0; i < channel[user[id_finder(token)].channel_id].users; i++) {
				if (channel[user[id_finder(token)].channel_id].user_id[i] == id_finder(token)) index = i;
			}
			for (int i = index + 1; i < channel[user[id_finder(token)].channel_id].users; i++) {
				channel[user[id_finder(token)].channel_id].user_id[i - 1] = channel[user[id_finder(token)].channel_id].user_id[i];
			}
			channel[user[id_finder(token)].channel_id].users--;
			sprintf(buffer, "{\"type\":\"Successful\",\"content\":\"\"}");
		}
		// Send the buffer to client
		send(client_socket, buffer, sizeof(buffer), 0);

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void logout() {	 /************************** Logout *****************************/

	char buffer[MAX];
	if (id_finder(token) == -1)	sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Authentication failed!\"}");
	else {
		sprintf(buffer, "{\"type\":\"Successful\",\"content\":\"\"}");
	}
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int online(char* name) {
	for (int i = 0; i < user_id; i++) {
		if (strcmp(user[i].username, name) == 0) return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void search_members() {

	int flag=0;
	char buffer[MAX];
	if (id_finder(token) == -1) 	sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Authentication failed!\"}");
	else if (user[id_finder(token)].channel_id == -1) sprintf(buffer, "{\"type\": \"Error\", \"content\": \"User is not online right now!\"}");
	else {
		for (int i = 0; i < channel[user[id_finder(token)].channel_id].users; i++) {
			if (strcmp(username, user[channel[user[id_finder(token)].channel_id].user_id[i]].username) == 0) { 
				sprintf(buffer, "{\"type\":\"result\",\"content\":\"true\"}");
				flag = 1;
			}
		}
		if(!flag)	sprintf(buffer, "{\"type\":\"result\",\"content\":\"false\"}");
	}

	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void search_message() {

	char buffer[10000];
	if (id_finder(token) == -1) 	sprintf(buffer, "{\"type\":\"Error\",\"content\":\"Authentication failed!\"}");
	else if (user[id_finder(token)].channel_id == -1) sprintf(buffer, "{\"type\":\"Error\",\"content\":\"User is not online right now!\"}");
	else {

		ckJSON *add, *content;

		/***************** Extracting messages from the file ********************/
		char filename[100];
		FILE* channelfile;
		sprintf(filename, "./Resources/Channels/%s.cyko", channel[user[id_finder(token)].channel_id].name);
		channelfile = fopen(filename, "r");
		char js[10000];
		fgets(js, 10000, channelfile);
		fclose(channelfile);

		/***************************** Finding Nemo (Messages)! ****************************/
		ckJSON* out = Parse_ckJSON(js);
		ckJSON *cont = GetObjectItem_ckJSON(out, "messages");
		int count = GetArraySize_ckJSON(cont);

		add = CreateObject_ckJSON();
		content = CreateArray_ckJSON();
		AddItemToObject_ckJSON(add, "type", CreateString_ckJSON("list"));
		AddItemToObject_ckJSON(add, "message", content);

		for (int i = 0; i < count; i++) {
			ckJSON* mas = GetArrayItem_ckJSON(cont, i);
			char* ms = GetObjectItem_ckJSON(mas, "content")->valuestring;
			char* p;
			p = strstr(ms, message);
			if (p) {

				/************************ Adding the found message to the JSON *************************/

				AddItemToArray_ckJSON(content, CreateString_ckJSON(ms));
			}
		}
		strcpy(buffer, PrintUnformatted_ckJSON(add));
		Delete_ckJSON(add);
	}
	// Send the buffer to client
	send(client_socket, buffer, sizeof(buffer), 0);

}
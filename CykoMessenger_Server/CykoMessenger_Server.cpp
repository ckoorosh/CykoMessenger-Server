/*
Cyko Server 2020 © All rights reserved
*/


#include "stdafx.h"
#include "Functions.c"
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include "cJSON.h"
#include <string.h>
#pragma comment(lib,"WS2_32")

int main()
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
			return 0;
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
			return 0;
		}
		else
			printf("Socket successfully bound...\n");

		// Now server is ready to listen and verify
		if ((listen(server_socket, 5)) != 0)
		{
			printf("Listen failed...\n");
			return 0;
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
				return 0;
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

    return 0;
}


/*
Cykomessenger 2019 © All rights reserved
With cooperation of 'geeksforgeeks','stackoverflow' and Ali Hatami
*/

#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
//#include "cJSON.h"
#include "Cyko_JSON.h"
//#include "cJSON.c"
#include "Cyko_JSON.c"
#include <string.h>
#include "functions.cpp"
#pragma comment(lib,"WS2_32")

int main()
{
	socket_connect();
	welcome_screen();
	Mainmenu();
	return 0;
}

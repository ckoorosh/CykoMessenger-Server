#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include "cJSON.h"
#include "Functions.c"
#include <string.h>
#pragma comment(lib,"WS2_32")

int main()
{
	socket_connect();
    return 0;
}


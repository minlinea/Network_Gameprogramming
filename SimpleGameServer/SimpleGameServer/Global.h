#pragma once
#include <vector>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <math.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

//���� ����




// ���� ���
const int MAX_PLAYER{ 3 };
const int MAP_ROW{ 16 };
const int MAP_COLUMN{ 16 };

const float TILEWIDTH{ 50 };
const float TILEHEIGHT{ 50 };

const ULONG MAX_SAMPLE_COUNT = 50;

#define SERVERPORT 9000
#define BUFSIZE 512


//// ����� ���� ���
#include "cTimer.h"
//#include "GameServerThread.h"
//#include "MatchingServer.h"

void err_quit(const char* msg);
void err_display(const char* msg);

// ���� �Լ� ���� ���
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
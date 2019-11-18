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

const ULONG MAX_SAMPLE_COUNT = 50;



















// 전역 상수
const int MAX_PLAYER{ 3 };
const int MAP_ROW{ 16 };
const int MAP_COLUMN{ 16 };

const float TILEWIDTH{ 50 };
const float TILEHEIGHT{ 50 };

// 사용자 정의 헤더
#include "GameServerThread.h"
#include "cTimer.h"


// 소켓 함수 오류 출력
inline void err_quit(const char* msg)
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

inline void err_display(const char* msg)
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
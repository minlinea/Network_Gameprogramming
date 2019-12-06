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

//전역 변수




// 전역 상수
const int MAX_PLAYER{ 3 };
const int MAP_ROW{ 16 };
const int MAP_COLUMN{ 16 };
const int MAX_BOMB{ 8 };
const float MAINTAIN_BOMBTIME{ 3 };
const float TILEWIDTH{ 50 };
const float TILEHEIGHT{ 50 };

#define SERVERPORT 9000
#define BUFSIZE 512

//// 사용자 정의 헤더
#include "cTimer.h"
#include "GameServerThread.h"
#include "MatchingServer.h"



enum ObjectType
{
	User = 0,
	Empty,
	Wall,
	Bomb,
	WaterStream
};


#pragma once

///////////////////////////////////////////////////////////////////////
#define WINDOW_WIDTH	1000
#define WINDOW_HEIGH	1000
#define MAP_ROW	16
#define MAP_COLUMN	16

#define MAX_PLAYER_NUM	3
#define NUM_OF_MENU 2

#define TitleSize 25
///////////////////////////////////////////////////////////////////////
struct Float2
{
	float x, y;
};

struct Float3
{
	float x, y, z;
};

struct Color4
{
	float r,g,b,a;
};
#pragma pack(1)
struct CharacterStatus
{
	Float2 position;
	char whoseControlNum;
};
#pragma pack()
#pragma pack(1)
struct KeyInput
{
	bool Up;
	bool Down;
	bool Left;
	bool Right;
	bool Attack;
};
#pragma pack()
#pragma pack(1)
struct FixedData
{
	bool mapChanged;
	char NumOfClient;
};
#pragma pack()

enum ObjectType 
{
	User = 0,
	Empty,
	Wall,
	Bomb,
	WaterStream
};

enum MSG_MatchingSystem
{
	Msg_Ready = 8,
	Msg_ReadyCancel,
	Msg_ConfirmReadyCancel,
	Msg_PlayGame
};

#pragma pack(1)
struct MapData
{
	char	m_Map[MAP_ROW*MAP_COLUMN];
};
#pragma pack()
#pragma once

///////////////////////////////////////////////////////////////////////
#define WINDOW_WIDTH	1000
#define WINDOW_HEIGH	1000
#define MAP_ROW	16
#define MAP_COLUMN	16

#define MAX_PLAYER_NUM	3
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
	bool notuse0;
	bool notuse1;
	bool notuse2;
	bool notuse3;
	bool notuse4;
	bool notuse5;
	bool notuse6;
	bool notuse7;
	bool notuse8;
	bool notuse9;
	bool notuse10;
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
	Player,
	Empty,
	Wall,
	Breakable_Wall,
	Item_AddSpeed,
	Item_AddBomb,
	Item_AddPower,
	Bomb_Anim1,		//기본 형태
	Bomb_Anim2,		//중간 크기
	Bomb_Anim3		//가장 작은 크기
};
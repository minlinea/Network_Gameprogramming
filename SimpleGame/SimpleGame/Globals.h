#pragma once
#define MAX_OBJ_COUNT 1000
#define HERO_ID 0
#define GRAVITY 9.8f

#define FAMOUNT 20.f;

#define TYPE_NORMAL 0
#define TYPE_BULLET 1

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
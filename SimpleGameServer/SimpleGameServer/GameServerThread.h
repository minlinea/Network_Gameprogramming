#pragma once
#include "Global.h"

// ����ü ���� ����
#pragma pack(1)
struct InputData
{
	bool Up;
	bool Down;
	bool Left;
	bool Right;
	bool attack;
	bool button0;
	bool button1;
	bool button2;
	bool button3;
	bool button4;
	bool button5;
	bool button6;
	bool button7;
	bool button8;
	bool button9;
	bool button10;
};
#pragma pack()

#pragma pack(1)
struct FixedData
{
	bool mapChanged;
	char NumOfClient;
};
#pragma pack()

#pragma pack(1)
struct PlayerData
{
	float x;
	float y;
	char n;
};
#pragma pack()

#pragma pack(1)
struct MapData
{
	char	m_Map[MAP_ROW*MAP_COLUMN];
};
#pragma pack()
// ����ü ���� ��


enum status
{
	dead, live
};

struct Player
{
public:
	float		x, y;
	float		fSpeed{ 10.f };
	status		stat;
	SOCKADDR_IN	sockAddress;
	InputData	KeyInput;
	// ��� ���� ������ �ʿ��� ����

	float		vec2[2];

public:
//	void move();
};


struct GameServerThreadData
{
public:
	Player					m_Players[3];				// �÷��̾� �������� ���� ����ü (���� ���α׷��ְ� ����)
	FixedData				m_fPacketH2C;
	MapData					m_MapData;
	char					m_cPlayerControl[MAX_PLAYER];

public:

	void MakeCommunicationThread(void);
	void Update(float fElapsedTime);
};

struct CommunicationThreadData
{
public:
	GameServerThreadData*	pGameData;
	char					cClientNumb;

	CommunicationThreadData(GameServerThreadData* pData, int n) :pGameData(pData), cClientNumb(n) {};
};

class GameServerThread
{
public:
	//GameServerThread();
	//~GameServerThread();
};


DWORD WINAPI GameServerThread(LPVOID arg);
//
//DWORD WINAPI ClientCommunicationThread(LPVOID arg);
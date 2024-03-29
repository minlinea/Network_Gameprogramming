#pragma once
#include "Global.h"

// 구조체 정의 시작
#pragma pack(1)
struct InputData
{
	bool Up = false;
	bool Down = false;
	bool Left = false; 
	bool Right = false;
	bool attack = false;
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
// 구조체 정의 끝


enum status
{
	dead, live
};

struct InfoBomb
{
	float ftime = 3.f;
	int x = -1, y = -1;
	bool alive = false;
	InfoBomb(int a, int b, float ft, bool al) :x(a), y(b), ftime(ft), alive(al) {	};
	InfoBomb() {};
};

struct Player
{
public:
	float		x, y;
	float		fSpeed{ 10.f };
	status		stat = live;
	SOCKET 	sockAddress;
	InputData	KeyInput;

	int iBomb = 0;
	InfoBomb HaveBomb[MAX_BOMB];
	//std::vector<InfoBomb> HaveBomb;
	
	// 등등 게임 로직에 필요한 변수
	float		vec2[2];

public:
//	void move();
};



struct GameServerThreadData
{
public:
	Player					m_Players[3];				// 플레이어 개개인의 상태 구조체 (socket 구조체도 갖고 있음)
	FixedData				m_fPacketH2C;
	MapData					m_MapData;
	char					m_cPlayerControl[MAX_PLAYER];

	bool					m_bGameEnd{ 0 };

public:

	void MakeCommunicationThread(void);
	void Update(float fElapsedTime);
};

struct CommunicationThreadData
{
public:
	GameServerThreadData*	pGameData;
	char					cClientNumb;

	CommunicationThreadData(GameServerThreadData* pData, char n) :pGameData(pData), cClientNumb(n) {};
};


DWORD WINAPI GameServerThread(LPVOID arg);
//
DWORD WINAPI ClientCommunicationThread(LPVOID arg);
#pragma once
#include "GameServerThread.h"


void GameServerThreadData::MakeCommunicationThread(void)
{
	

	for (int i = 0; i < m_fPacketH2C.NumOfClient; ++i)
	{
		HANDLE hThread{ nullptr };
		CommunicationThreadData cData(this, i);
		hThread = CreateThread(NULL, 0, ClientCommunicationThread, (LPVOID)&cData, 0, NULL);

		if (hThread == NULL)
		{
			//
		}
		else
		{
			CloseHandle(hThread);
		}
	}
}

void GameServerThreadData::Update(float fTimeElapsed)
{
	// 함수화 안하고 그냥 작성함
	m_fPacketH2C.mapChanged = 0;

	for (int i = 0; i < m_fPacketH2C.NumOfClient; ++i)
	{
		m_Players[i].vec2[0] = 0.f;
		m_Players[i].vec2[1] = 0.f;

		if (m_Players[i].KeyInput.Up)
		{
			m_Players[i].vec2[0] += 1.f;
		}
		if (m_Players[i].KeyInput.Down)
		{
			m_Players[i].vec2[0] -= 1.f;
		}
		if (m_Players[i].KeyInput.Left)
		{
			m_Players[i].vec2[1] += 1.f;
		}
		if (m_Players[i].KeyInput.Right)
		{
			m_Players[i].vec2[1] -= 1.f;
		}

		float posX = m_Players[i].vec2[0] * m_Players[i].fSpeed * fTimeElapsed + m_Players[i].x;
		float posY = m_Players[i].vec2[1] * m_Players[i].fSpeed * fTimeElapsed + m_Players[i].y;
		bool collision = 0;

		for (int a = 0; a < MAP_ROW; ++a)
			for (int b = 0; b < MAP_COLUMN; ++b)
			{
				if (posX > b*TILEWIDTH&&posX<(b + 1)*TILEWIDTH&&posY>a*TILEHEIGHT&&posY < a*TILEHEIGHT)
				{
					switch (m_MapData.m_Map[a*MAP_COLUMN + b])
					{
					case 0:
						break;
					case 1:
						collision = 1;
						a = MAP_ROW;
						b = MAP_COLUMN;
						break;
					default:
						break;
					}
				}
			}

		if (!collision)
		{
			m_Players[i].x = posX;
			m_Players[i].y = posY;
		}
	}


}

DWORD WINAPI GameServerThread(LPVOID arg)
{
	// arg 를 통해서 매칭룸의 플레이어들을 전달받을 것이다
	// arg 는 소켓배열 포인터 등이 될 것이다
	// arg 를 통해서 gameData를 생성하는 것이다
	// 이 부분은 추후에 매칭 기능과 합쳐질 때 작성한다

	SOCKET* s = (SOCKET*)(arg);

	CGameTimer timer;
	GameServerThreadData gameData;

	gameData.m_Players[0].sockAddress = s[0];
	gameData.m_Players[1].sockAddress = s[1];
	gameData.m_Players[2].sockAddress = s[2];

	for (int i = 0; i < MAX_PLAYER; ++i)
		gameData.m_cPlayerControl[i] = i;

	gameData.m_fPacketH2C.mapChanged = 1;
	gameData.m_fPacketH2C.NumOfClient = 3;
	gameData.MakeCommunicationThread();

	Sleep(2000);

	while (1)
	{
		timer.Tick(0.0f);
		// 게임 업데이트

		Sleep(10);

		gameData.Update(timer.GetTimeElapsed());
	}
}

DWORD WINAPI ClientCommunicationThread(LPVOID arg)
{
	CommunicationThreadData* pCommData = (CommunicationThreadData*)arg;

	GameServerThreadData* pGameData = pCommData->pGameData;
	int clientNumber = pCommData->cClientNumb;

	SOCKET client_sock = pGameData->m_Players[clientNumber].sockAddress;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;


	PlayerData playerData;

	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);

	while (1)
	{
		// 데이터 받기
		retval = recv(client_sock, (char*)&pGameData->m_Players[clientNumber].KeyInput, sizeof(InputData), 0);
		if (retval == SOCKET_ERROR)
		{
			//err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;




		// 데이터 보내기에 앞서 서버가 연산 중인 데이터의 한 순간을 복사시켜서
		// 복사본을 전송하도록 한다
		// 왜 이러냐면 고정부 가변부 나눠서 데이터를 보낼건데
		// 지금 보내는 데이터는 보내는 동시에 서버에서 값이 수정되기 때문이다

		GameServerThreadData gData(*pGameData);




		// 맵변화,유저수 보내기
		retval = send(client_sock, (char*)&gData.m_fPacketH2C, sizeof(FixedData), 0);
		if (retval == SOCKET_ERROR)
		{
			//err_display("send()");
			break;
		}

		// 맵보내기
		if (gData.m_fPacketH2C.mapChanged)
		{
			retval = send(client_sock, (char*)&gData.m_MapData, sizeof(MapData), 0);
		}

		// 유저 수 만큼의 개별 유저 정보 보내기
		for (int i = 0; i < gData.m_fPacketH2C.NumOfClient; ++i)
		{
			playerData.x = gData.m_Players[i].x;
			playerData.y = gData.m_Players[i].y;
			playerData.n = gData.m_cPlayerControl[clientNumber];

			retval = send(client_sock, (char*)& playerData, sizeof(PlayerData), 0);
			if (retval == SOCKET_ERROR)
			{
				//err_display("send()");
				break;
			}
		}
	}

	//closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}
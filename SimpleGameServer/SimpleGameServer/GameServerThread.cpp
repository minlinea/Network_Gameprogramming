#pragma once
#include "GameServerThread.h"

float ChangeTime = 5.f;
float Now_Time = 0.f;
void GameServerThreadData::MakeCommunicationThread(void)
{
	

	for (int i = 0; i < m_fPacketH2C.NumOfClient; ++i)
	{
		HANDLE hThread{ nullptr };
		CommunicationThreadData* cData = new CommunicationThreadData(this, i);
		hThread = CreateThread(NULL, 0, ClientCommunicationThread, (LPVOID)cData, 0, NULL);

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
	// �Լ�ȭ ���ϰ� �׳� �ۼ���
	int death_cnt = 0;

	for (int i = 0; i < m_fPacketH2C.NumOfClient; ++i)
	{
		m_Players[i].vec2[0] = 0.f;
		m_Players[i].vec2[1] = 0.f;

		if (m_Players[i].KeyInput.Up)
		{
			m_Players[i].vec2[1] -= 1.f;
		}
		if (m_Players[i].KeyInput.Down)
		{
			m_Players[i].vec2[1] += 1.f;
		}
		if (m_Players[i].KeyInput.Left)
		{
			m_Players[i].vec2[0] -= 1.f;
		}
		if (m_Players[i].KeyInput.Right)
		{
			m_Players[i].vec2[0] += 1.f;
		}

		float posX = m_Players[i].vec2[0] * m_Players[i].fSpeed * fTimeElapsed + m_Players[i].x;
		float posY = m_Players[i].vec2[1] * m_Players[i].fSpeed * fTimeElapsed + m_Players[i].y;
		bool collision = false;

		int posA, posB;
		for (int a = 0; a < MAP_ROW; ++a)
		{
			for (int b = 0; b < MAP_COLUMN; ++b)
			{
				if (posX + MAP_ROW / 2 > b && posX + MAP_ROW / 2 < (b + 1)
					&& posY + MAP_COLUMN / 2 > a && posY + MAP_COLUMN / 2 < a + 1)
				{
					posA = a;
					posB = b;
					a = MAP_ROW;
					b = MAP_COLUMN;
				}
			}
		}

		switch (m_MapData.m_Map[posA*MAP_COLUMN + posB])
		{
		case User:
			break;
		case Empty:
			break;
		case Bomb:
			break;
		case WaterStream:		// ��ź ���ٱ�
			m_Players->stat = dead;
			break;
		default:
			collision = true;
			break;
		}
		if (!collision)
		{
			m_Players[i].x = posX;
			m_Players[i].y = posY;
		}

		if (m_Players[i].KeyInput.attack)		// ��ź(5) �α�
		{
			switch (m_MapData.m_Map[posA*MAP_COLUMN + posB])
			{
			case Empty:
				if (m_Players[i].HaveBomb.size() < MAX_BOMB)
				{
					m_Players[i].HaveBomb.emplace_back(posA,posB);
					m_MapData.m_Map[posA*MAP_COLUMN + posB] = Bomb;
					m_fPacketH2C.mapChanged = true;
				}
				break;
			default:
				m_fPacketH2C.mapChanged = false;
				break;
			}
		}
		
		//��ź ó��
		for (auto p = m_Players[i].HaveBomb.begin(); p != m_Players[i].HaveBomb.end(); ++p)
		{
			p->ftime += fTimeElapsed;
			if (p->ftime > MAINTAIN_BOMBTIME + 0.5f)
			{
				m_MapData.m_Map[p->x*MAP_COLUMN + p->y] = Empty;
				
				int bx = p->x;
				int by = p->y;
				for (int t = 0; t < BOMB_POWER; ++t)
				{
					if (bx + t >= MAP_ROW)
						break;
					else if (m_MapData.m_Map[(bx + t)*MAP_COLUMN + by] == Wall)
						break;
					else
						m_MapData.m_Map[(bx + t)*MAP_COLUMN + by] = Empty;
				}
				for (int t = 0; t < BOMB_POWER; ++t)
				{
					if (bx - t <= -1)
						break;
					else if (m_MapData.m_Map[(bx - t)*MAP_COLUMN + by] == Wall)
						break;
					else
						m_MapData.m_Map[(bx - t)*MAP_COLUMN + by] = Empty;
				}
				for (int t = 0; t < BOMB_POWER; ++t)
				{
					if (by + t >= MAP_COLUMN)
						break;
					else if (m_MapData.m_Map[bx*MAP_COLUMN + (by + t)] == Wall)
						break;
					else
						m_MapData.m_Map[bx*MAP_COLUMN + (by + t)] = Empty;
				}
				for (int t = 0; t < BOMB_POWER; ++t)
				{
					if (by - t <= -1)
						break;
					else if (m_MapData.m_Map[bx*MAP_COLUMN + (by - t)] == Wall)
						break;
					else
						m_MapData.m_Map[bx*MAP_COLUMN + (by - t)] = Empty;
				}
				p->alive = false;
				m_fPacketH2C.mapChanged = true;
			}
			else if (p->ftime > MAINTAIN_BOMBTIME)
			{
				m_MapData.m_Map[p->x*MAP_COLUMN + p->y] = WaterStream;
				int bx = p->x;
				int by = p->y;
				for (int t = 0; t < BOMB_POWER; ++t)
				{
					if (bx + t >= MAP_ROW)
						break;
					else if (m_MapData.m_Map[(bx + t)*MAP_COLUMN + by] == Wall)
						break;
					else
						m_MapData.m_Map[(bx + t)*MAP_COLUMN + by] = WaterStream;
				}
				for (int t = 0; t < BOMB_POWER; ++t)
				{
					if (bx - t <= -1)
						break;
					else if (m_MapData.m_Map[(bx - t)*MAP_COLUMN + by] == Wall)
						break;
					else
						m_MapData.m_Map[(bx - t)*MAP_COLUMN + by] = WaterStream;
				}
				for (int t = 0; t < BOMB_POWER; ++t)
				{
					if (by + t >= MAP_COLUMN)
						break;
					else if (m_MapData.m_Map[bx*MAP_COLUMN + (by + t)] == Wall)
						break;
					else
						m_MapData.m_Map[bx*MAP_COLUMN + (by + t)] = WaterStream;
				}
				for (int t = 0; t < BOMB_POWER; ++t)
				{
					if (by - t <= -1)
						break;
					else if (m_MapData.m_Map[bx*MAP_COLUMN + (by - t)] == Wall)
						break;
					else
						m_MapData.m_Map[bx*MAP_COLUMN + (by - t)] = WaterStream;
				}

				m_fPacketH2C.mapChanged = true;
			}
			else
				m_MapData.m_Map[p->x*MAP_COLUMN + p->y] = Bomb;
		}
		for (auto t = m_Players[i].HaveBomb.begin(); t < m_Players[i].HaveBomb.end(); ++t)
		{
			if (!t->alive)
			{
				t = m_Players[i].HaveBomb.erase(t);
					break;
			}
		}
		if (m_MapData.m_Map[posA*MAP_COLUMN + posB] == WaterStream)
			m_Players[i].stat = dead;

		if (m_Players[i].stat == dead)
			death_cnt += 1;
	}
	Now_Time += fTimeElapsed;
	if (Now_Time > ChangeTime)
	{
		float tempx[3], tempy[3];
		int t = rand() % 3;
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			tempx[i] = m_Players[(i+t) % 3].x;
			tempy[i] = m_Players[(i+t) % 3].y;
		}
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			m_Players[(i) % 3].x = tempx[i];
			m_Players[(i) % 3].y = tempy[i];
		}
		Now_Time = 0.f;
	}


}

DWORD WINAPI GameServerThread(LPVOID arg)
{
	// arg �� ���ؼ� ��Ī���� �÷��̾���� ���޹��� ���̴�
	// arg �� ���Ϲ迭 ������ ���� �� ���̴�
	// arg �� ���ؼ� gameData�� �����ϴ� ���̴�
	// �� �κ��� ���Ŀ� ��Ī ��ɰ� ������ �� �ۼ��Ѵ�

	SOCKET* s = (SOCKET*)(arg);

	CGameTimer timer;
	GameServerThreadData gameData;

	gameData.m_Players[0].sockAddress = s[0];
	gameData.m_Players[1].sockAddress = s[1];
	gameData.m_Players[2].sockAddress = s[2];
	gameData.m_Players[0].x = 0.10f;
	gameData.m_Players[0].y = 0.20f;
	gameData.m_Players[1].x = 0.30f;
	gameData.m_Players[1].y = 0.40f;
	gameData.m_Players[2].x = 0.50f;
	gameData.m_Players[2].y = 0.60f;
	
	for (int i = 0; i < MAX_PLAYER; ++i)
		gameData.m_cPlayerControl[i] = i;

	gameData.m_fPacketH2C.mapChanged = false;
	gameData.m_fPacketH2C.NumOfClient = 3;

		int map_arr[MAP_COLUMN][MAP_ROW] = {
		{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
		{2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,2},
		{2,2,2,1,1,2,1,2,1,2,1,2,2,2,1,2},
		{2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
		{2,2,1,2,1,1,2,1,1,1,1,2,2,1,2,2},
		{2,2,1,1,1,2,2,1,1,2,1,1,1,1,2,2},
		{2,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2},
		{2,2,1,1,1,1,2,1,1,2,1,1,1,1,2,2},
		{2,1,1,2,1,1,2,1,1,1,1,2,1,1,1,2},
		{2,2,1,1,1,1,1,1,1,2,1,1,1,1,2,2},
		{2,1,1,2,1,1,2,1,1,2,1,2,1,1,1,2},
		{2,2,1,1,1,1,2,1,1,1,1,1,2,1,2,2},
		{2,1,1,2,1,2,1,1,1,1,1,1,1,1,1,2},
		{2,2,1,1,1,1,1,2,1,2,1,1,2,1,2,2},
		{2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2},
		{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	};
	gameData.m_MapData;
	for (int i = 0; i < MAP_COLUMN; ++i)
	{
		for (int j = 0; j < MAP_ROW; ++j)
		{
			gameData.m_MapData.m_Map[i*MAP_COLUMN+j] = map_arr[i][j];
		}
	}

	Sleep(2000);

	gameData.MakeCommunicationThread();

	gameData.m_fPacketH2C.mapChanged = true;
	while (1)
	{
		//printf("������Ʈ\n");

		timer.Tick(0.0f);
		// ���� ������Ʈ

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

	//Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);
	printf("%d", clientaddr);
	while (1)
	{
		Sleep(10);
		// ������ �ޱ�
		retval = recv(client_sock, (char*)&pGameData->m_Players[clientNumber].KeyInput, sizeof(InputData), 0);
		if (retval == SOCKET_ERROR)
		{
			//err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ������ �����⿡ �ռ� ������ ���� ���� �������� �� ������ ������Ѽ�
		// ���纻�� �����ϵ��� �Ѵ�
		// �� �̷��ĸ� ������ ������ ������ �����͸� �����ǵ�
		// ���� ������ �����ʹ� ������ ���ÿ� �������� ���� �����Ǳ� �����̴�

		GameServerThreadData gData(*pGameData);


		// �ʺ�ȭ,������ ������
		retval = send(client_sock, (char*)&gData.m_fPacketH2C, sizeof(FixedData), 0);
		if (retval == SOCKET_ERROR)
		{
			//err_display("send()");
			break;
		}

		// �ʺ�����
		if (gData.m_fPacketH2C.mapChanged)
		{
			retval = send(client_sock, (char*)&gData.m_MapData, sizeof(MapData), 0);
		}

		// ���� �� ��ŭ�� ���� ���� ���� ������
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
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}


/*
��ź ���� Ŭ���� �߰�?
ĳ���� Ŭ����?
	������ �� �ִ� �ִ� ��ź num�� ���� ���ǰ� �ִ� ��ź num ����
	���ٱ� ����
	
	��ź Ÿ�̸� ����


���������尡 �����ϴ� ĳ���� ����� Ÿ�̸� �߰�

attack�� ������ true?

update ���� ����
*/
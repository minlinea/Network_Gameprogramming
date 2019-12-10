#pragma once
#include "GameServerThread.h"

float ChangeTime = 5.f;
float Now_Time = 0.f;
extern HANDLE hCommunicationThread[3];
extern HANDLE hCommunicationThreadEvent[3];
void GameServerThreadData::MakeCommunicationThread(void)
{
	

	for (int i = 0; i < m_fPacketH2C.NumOfClient; ++i)
	{
		HANDLE hThread{ nullptr };
		hCommunicationThreadEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (hCommunicationThreadEvent[i] != NULL)
		{
			CommunicationThreadData* cData = new CommunicationThreadData(this, i);
			hCommunicationThread[i] = CreateThread(NULL, 0, ClientCommunicationThread, (LPVOID)cData, 0, NULL);

			if (hCommunicationThread[i] == NULL)
			{
				//
			}
			else
			{
				CloseHandle(hCommunicationThread[i]);
			}
		}
	}
}

void GameServerThreadData::Update(float fTimeElapsed)
{
	// 함수화 안하고 그냥 작성함
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
				if (posX + MAP_ROW / 2 >= b && posX + MAP_ROW / 2 <= (b + 1)
					&& posY + MAP_COLUMN / 2 >= a && posY + MAP_COLUMN / 2 <= a + 1)
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
		case WaterStream:		// 폭탄 물줄기
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

		
		if (m_Players[i].KeyInput.attack)		// 폭탄(5) 두기
		{
			switch (m_MapData.m_Map[posA*MAP_COLUMN + posB])
			{
			case Empty:
				if (m_Players[i].iBomb < MAX_BOMB)
				{

					m_Players[i].HaveBomb[m_Players[i].iBomb] = { posA, posB, 0.f, true };
					m_Players[i].iBomb += 1;
					m_MapData.m_Map[posA*MAP_COLUMN + posB] = Bomb;
					m_fPacketH2C.mapChanged = true;
				}
				break;
			default:
				m_fPacketH2C.mapChanged = false;
				break;
			}
		}
		for (int k=0; k< m_Players[i].iBomb; ++k)
		{
			if (m_Players[i].HaveBomb[k].alive)
			{
				m_Players[i].HaveBomb[k].ftime += fTimeElapsed;
				int bx = m_Players[i].HaveBomb[k].x;
				int by = m_Players[i].HaveBomb[k].y;
				if (m_Players[i].HaveBomb[k].ftime > MAINTAIN_BOMBTIME + 0.5f)
				{
					m_MapData.m_Map[bx*MAP_COLUMN + by] = Empty;

					for (int t = 0; t < BOMB_POWER; ++t)
					{
						if (bx + t > MAP_ROW - 1)
							break;
						else if (m_MapData.m_Map[(bx + t)*MAP_COLUMN + by] == Wall)
							break;
						else
							m_MapData.m_Map[(bx + t)*MAP_COLUMN + by] = Empty;
					}
					for (int t = 0; t < BOMB_POWER; ++t)
					{
						if (bx - t < 0)
							break;
						else if (m_MapData.m_Map[(bx - t)*MAP_COLUMN + by] == Wall)
							break;
						else
							m_MapData.m_Map[(bx - t)*MAP_COLUMN + by] = Empty;
					}
					for (int t = 0; t < BOMB_POWER; ++t)
					{
						if (by + t > MAP_COLUMN - 1)
							break;
						else if (m_MapData.m_Map[bx*MAP_COLUMN + (by + t)] == Wall)
							break;
						else
							m_MapData.m_Map[bx*MAP_COLUMN + (by + t)] = Empty;
					}
					for (int t = 0; t < BOMB_POWER; ++t)
					{
						if (by - t < 0)
							break;
						else if (m_MapData.m_Map[bx*MAP_COLUMN + (by - t)] == Wall)
							break;
						else
							m_MapData.m_Map[bx*MAP_COLUMN + (by - t)] = Empty;
					}
					m_Players[i].HaveBomb[k].alive = false;
					m_fPacketH2C.mapChanged = true;
				}
				else if (m_Players[i].HaveBomb[k].ftime > MAINTAIN_BOMBTIME)
				{
					m_MapData.m_Map[bx*MAP_COLUMN + by] = WaterStream;
					for (int t = 0; t < BOMB_POWER; ++t)
					{
						if (bx + t > MAP_ROW - 1)
							break;
						else if (m_MapData.m_Map[(bx + t)*MAP_COLUMN + by] == Wall)
							break;
						else
							m_MapData.m_Map[(bx + t)*MAP_COLUMN + by] = WaterStream;
					}
					for (int t = 0; t < BOMB_POWER; ++t)
					{
						if (bx - t < 0)
							break;
						else if (m_MapData.m_Map[(bx - t)*MAP_COLUMN + by] == Wall)
							break;
						else
							m_MapData.m_Map[(bx - t)*MAP_COLUMN + by] = WaterStream;
					}
					for (int t = 0; t < BOMB_POWER; ++t)
					{
						if (by + t > MAP_COLUMN - 1)
							break;
						else if (m_MapData.m_Map[bx*MAP_COLUMN + (by + t)] == Wall)
							break;
						else
							m_MapData.m_Map[bx*MAP_COLUMN + (by + t)] = WaterStream;
					}
					for (int t = 0; t < BOMB_POWER; ++t)
					{
						if (by - t < 0)
							break;
						else if (m_MapData.m_Map[bx*MAP_COLUMN + (by - t)] == Wall)
							break;
						else
							m_MapData.m_Map[bx*MAP_COLUMN + (by - t)] = WaterStream;
					}

					m_fPacketH2C.mapChanged = true;
				}
				else
					m_MapData.m_Map[bx*MAP_COLUMN + by] = Bomb;
			}
		}
		for (int t = 0; t < m_Players[i].iBomb; ++t)
		{
			if (!m_Players[i].HaveBomb[t].alive)
			{
				for (int k = 0; k < m_Players[i].iBomb - 1; ++k)
				{
					m_Players[i].HaveBomb[k] = m_Players[i].HaveBomb[k + 1];
				}
				m_Players[i].HaveBomb[m_Players[i].iBomb - 1] = { -1, -1, 0.f, false };
				m_Players[i].iBomb += -1;
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




/*
폭탄 관련 클래스 추가?
캐릭터 클래스?
	보유할 수 있는 최대 폭탄 num과 현재 사용되고 있는 폭탄 num 보유
	물줄기 길이
	
	폭탄 타이머 보유


서버스레드가 관리하는 캐릭터 변경용 타이머 추가

attack이 무조건 true?

update 구조 변경
*/
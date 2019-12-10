#pragma once
#include "GameServerThread.h"


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
	// 함수화 안하고 그냥 작성함
	int death_cnt = 0;
	float ChangeTime = 5.f;
	static float Now_Time = 0.f;
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

		switch (m_MapData.m_Map[posA * MAP_COLUMN + posB])
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
		if (!collision)		//충돌하지 않았으면 위치 갱신
		{
			m_Players[i].x = posX;
			m_Players[i].y = posY;
		}


		if (m_Players[i].KeyInput.attack)		// 폭탄(5) 두기
		{
			switch (m_MapData.m_Map[posA * MAP_COLUMN + posB])
			{
			case Empty:
				if (m_Players[i].iBomb < MAX_BOMB)
				{
					m_Players[i].HaveBomb[m_Players[i].iBomb] = { posA, posB, 0.f, true };
					m_Players[i].iBomb += 1;
					m_MapData.m_Map[posA * MAP_COLUMN + posB] = Bomb;
					m_fPacketH2C.mapChanged = true;
				}
				break;
			default:
				m_fPacketH2C.mapChanged = false;
				break;
			}
		}
		for (int k = 0; k < m_Players[i].iBomb; ++k)
		{
			if (m_Players[i].HaveBomb[k].alive)															//폭탄이 유지되고 있다면
			{
				m_Players[i].HaveBomb[k].ftime += fTimeElapsed;									//폭탄의 유지시간 증가
				int bx = m_Players[i].HaveBomb[k].x;
				int by = m_Players[i].HaveBomb[k].y;
				if (m_Players[i].HaveBomb[k].ftime > MAINTAIN_BOMBTIME + 0.5f)			//물줄기 유지시간 종료 후
				{
					m_MapData.m_Map[bx * MAP_COLUMN + by] = Empty;					// 해당 지역 빈 곳으로 변경

					for (int t = 0; t < BOMB_POWER; ++t)											// 우측 검사
					{
						if (bx + t > MAP_ROW - 1)														//범위 바깥
							break;
						else if (m_MapData.m_Map[(bx + t) * MAP_COLUMN + by] == Wall)// 벽
							break;
						else																					//이외는 빈 곳으로 변경
							m_MapData.m_Map[(bx + t) * MAP_COLUMN + by] = Empty;
					}
					for (int t = 0; t < BOMB_POWER; ++t)											// 좌측 검사
					{
						if (bx - t < 0)																		  //범위 바깥
							break;
						else if (m_MapData.m_Map[(bx - t) * MAP_COLUMN + by] == Wall) // 벽
							break;
						else																				   //이외는 빈 곳으로 변경
							m_MapData.m_Map[(bx - t) * MAP_COLUMN + by] = Empty;
					}
					for (int t = 0; t < BOMB_POWER; ++t)												// 하단 검사
					{
						if (by + t > MAP_COLUMN - 1)												     //범위 바깥
							break;
						else if (m_MapData.m_Map[bx * MAP_COLUMN + (by + t)] == Wall)	 // 벽
							break;
						else																						// 이외는 빈 곳으로 변경
							m_MapData.m_Map[bx * MAP_COLUMN + (by + t)] = Empty;
					}
					for (int t = 0; t < BOMB_POWER; ++t)												// 상단 검사
					{
						if (by - t < 0)																			   //범위 바깥
							break;
						else if (m_MapData.m_Map[bx * MAP_COLUMN + (by - t)] == Wall)	  // 벽
							break;
						else																					   // 이외는 빈 곳으로 변경
							m_MapData.m_Map[bx * MAP_COLUMN + (by - t)] = Empty;
					}
					m_Players[i].HaveBomb[k].alive = false;											// 폭탄을 죽은 상태로 변경
					m_fPacketH2C.mapChanged = true;												// 맵 변경을 알림
				}
				else if (m_Players[i].HaveBomb[k].ftime > MAINTAIN_BOMBTIME)				//폭탄 유지시간 종료 후 물줄기로 변경
				{
					m_MapData.m_Map[bx * MAP_COLUMN + by] = WaterStream;				//해당 지역 물줄기
					for (int t = 0; t < BOMB_POWER; ++t)														//우측 검사
					{
						if (bx + t > MAP_ROW - 1)																	// 범위 바깥
							break;
						else if (m_MapData.m_Map[(bx + t) * MAP_COLUMN + by] == Wall)			// 벽
							break;
						else																							   //이외는 모두 물줄기
							m_MapData.m_Map[(bx + t) * MAP_COLUMN + by] = WaterStream;
					}
					for (int t = 0; t < BOMB_POWER; ++t)														//좌측 검사
					{
						if (bx - t < 0)																					// 범위 바깥
							break;
						else if (m_MapData.m_Map[(bx - t) * MAP_COLUMN + by] == Wall)		 		// 벽
							break;
						else																						       //이외는 모두 물줄기
							m_MapData.m_Map[(bx - t) * MAP_COLUMN + by] = WaterStream;
					}
					for (int t = 0; t < BOMB_POWER; ++t)															//하단 검사
					{
						if (by + t > MAP_COLUMN - 1)														   	// 범위 바깥
							break;
						else if (m_MapData.m_Map[bx * MAP_COLUMN + (by + t)] == Wall)				// 벽
							break;
						else																							      //이외는 모두 물줄기
							m_MapData.m_Map[bx * MAP_COLUMN + (by + t)] = WaterStream;
					}
					for (int t = 0; t < BOMB_POWER; ++t)														//상단 검사
					{
						if (by - t < 0)																				  	// 범위 바깥
							break;
						else if (m_MapData.m_Map[bx * MAP_COLUMN + (by - t)] == Wall)		 	// 벽
							break;
						else																						      //이외는 모두 물줄기
							m_MapData.m_Map[bx * MAP_COLUMN + (by - t)] = WaterStream;
					}

					m_fPacketH2C.mapChanged = true;
				}
				else
					m_MapData.m_Map[bx * MAP_COLUMN + by] = Bomb;							// 이외는 모두 폭탄 상태
			}
		}
		for (int t = 0; t < m_Players[i].iBomb; ++t)												// 폭탄의 GARBAGE CHECK	
		{
			if (!m_Players[i].HaveBomb[t].alive)												
			{
				for (int k = 0; k < m_Players[i].iBomb - 1; ++k)
				{
					m_Players[i].HaveBomb[k] = m_Players[i].HaveBomb[k + 1];
				}
				m_Players[i].HaveBomb[m_Players[i].iBomb - 1] = { -1, -1, 0.f, false };		//초기화
				m_Players[i].iBomb += -1;
				break;
			}

		}

		if (m_MapData.m_Map[posA * MAP_COLUMN + posB] == WaterStream)
			m_Players[i].stat = dead;

		if (m_Players[i].stat == dead)
			death_cnt += 1;
	}
	Now_Time += fTimeElapsed;
	if (Now_Time > ChangeTime)						//시간마다 위치 변환
	{
		float tempx[3], tempy[3];
		int t = rand() % 3;
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			tempx[i] = m_Players[(i + t) % 3].x;
			tempy[i] = m_Players[(i + t) % 3].y;
		}
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			m_Players[(i) % 3].x = tempx[i];
			m_Players[(i) % 3].y = tempy[i];
		}
		Now_Time = 0.f;
	}

	if (death_cnt >= MAX_PLAYER - 1)
		m_bGameEnd = true;


}

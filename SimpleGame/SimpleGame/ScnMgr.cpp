#include "stdafx.h"
#include "ScnMgr.h"
#include "Dependencies\freeglut.h"
#include <float.h>


ScnMgr::ScnMgr()
{
	
	// Initialize Renderer
	m_Renderer = new Renderer(WINDOW_WIDTH, WINDOW_HEIGH);		// 10x10m 방
	if (!m_Renderer->IsInitialized())
	{
		std::cout << "Renderer could not be initialized.. \n";
	}
	///////////////////Mathing////////////////////////////
	unsigned char msg;
	m_client = new TCPClient();

	//msg = Msg_Ready;
	//while (msg != Msg_PlayGame) {
	//	m_client->TitleSceneSendData(msg);

	//	m_client->TitleSceneRecvData(&msg);
	//	if (msg == Msg_ConfirmReadyCancel)
	//		delete m_client;
	//}
	////////////////////////////////////////////////////////////////
	//Initialize objects
	for (int i = 0; i < MAX_PLAYER_NUM; ++i)
	{
		m_Player[i] = NULL;
	}
	//가급적 수동으로 지우는건 지양하지만, 초기화 할 때는 괜찮다.

	for (int i = 0; i < MAX_PLAYER_NUM; ++i) {
		m_Player[i] = new Object();
		m_Player[i]->SetPos({ 0,0 });
		m_Player[i]->SetVol({ 1,1 });
		if(i==0)
			m_Player[i]->SetColor({ 1, 0, 0, 1 });
		else if (i == 1)
			m_Player[i]->SetColor({ 0, 1, 0, 1 });
		else if (i == 2)
			m_Player[i]->SetColor({ 0, 0, 1, 1 });
		m_Player[i]->SetVel({ 1,1 });
			  
		m_Player[i]->SetType(Player);
	}
	for (int i = 0; i < MAP_COLUMN; ++i)
	{
		for (int j = 0; j < MAP_ROW; ++j)
		{
			m_MapObject[i][j] = new Object();
			m_MapObject[i][j]->SetPos({ (float)(i- MAP_COLUMN/2),(float)(j - MAP_COLUMN / 2) });
			m_MapObject[i][j]->SetVol({ 1,1 });
			m_MapObject[i][j]->SetVel({ 0,0 });
			if (i == 0 || i == MAP_COLUMN - 1 || j == 0 || j == MAP_ROW - 1) {
				m_MapObject[i][j]->SetColor({ 1, 1, 1, 1 });

				m_MapObject[i][j]->SetType(Wall);
			}
			else
			{
				m_MapObject[i][j]->SetColor({ 0, 0, 0, 1 });

				m_MapObject[i][j]->SetType(Empty);
			}
		}
	}
}


ScnMgr::~ScnMgr()
{
	if (m_Renderer != NULL)
	{
		delete m_Renderer;
		m_Renderer = NULL;
	}
}


void ScnMgr::RenderScene(float ElapsedTime)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
	Float2 pos;
	Float2 vol;
	Color4 col;
	float interval= 50;
	ObjectType type;

	for (int i = 0; i < MAX_PLAYER_NUM; ++i) {
		m_Player[i]->GetPos(&pos);
		m_Player[i]->GetVol(&vol);
		m_Player[i]->GetColor(&col);
		float body_gap = interval / 4;
		m_Renderer->DrawSolidRect(pos.x * interval, pos.y * interval + body_gap, 0, vol.x * body_gap, 1, 1, 1, col.a);
		m_Renderer->DrawSolidRect(pos.x * interval, pos.y * interval - body_gap, 0, vol.x * body_gap * 3, col.r, col.g, col.b, col.a);
	}

	for (int i = 0; i < MAP_COLUMN; ++i)
	{
		for (int j = 0; j < MAP_ROW; ++j)
		{
			m_MapObject[i][j]->GetPos(&pos);
			m_MapObject[i][j]->GetVol(&vol);
			m_MapObject[i][j]->GetColor(&col);
			m_MapObject[i][j]->GetType(&type);

			switch (type)
			{
			case Wall:
				m_Renderer->DrawSolidRect(pos.x * interval, pos.y * interval, 0, vol.x * interval, 1, 1, 1, col.a);
				break;
			}
		}
	}
}

void ScnMgr::DeleteObject(int idx)
{
	if (idx < 0)
	{
		std::cout << "input idx is negative : " << idx << std::endl;
		return;
	}

	//if (idx >= MAX_OBJ_COUNT)
	//{
	//	std::cout << "input idx exceeds MAX_OBJ_COUNT : " << idx << std::endl;
	//	return;
	//}

	if (m_Player[idx] == NULL)
	{
		std::cout << "m_Player[" << idx << "] is NULL" << std::endl;
		return;
	}

	delete m_Player[idx];
	m_Player[idx] = NULL;
	return;
}

Object* ScnMgr::GetObj() const
{
	return *m_Player;
}

void ScnMgr::Update(float ElapsedTime)
{
	//character control
	std::cout << "Up: " << m_key.Up
		<< ", Down: " << m_key.Down
		<< ", Left: " << m_key.Left
		<< ", Right: " << m_key.Right << std::endl;


	CharacterStatus pos[3];

	m_client->PlaySceneSendData(m_key);
	m_client->PlaySceneRecvData(pos);
	ObjectType type;
	for (int i = 0; i < MAX_PLAYER_NUM; ++i) {
		m_Player[i]->Update(pos[i], ElapsedTime);
	}

}

void ScnMgr::DoGarbageCollection()
{
	/*for (int i = 0; i < MAX_OBJ_COUNT; ++i)
	{
		if (m_Player[i] != NULL)
		{
			int type;
			m_Player[i]->GetType(&type);
			if (type == TYPE_BULLET)
			{
				Float2 v;
				m_Player[i]->GetVel(&v);
				float vSize = sqrtf(v.x * v.x + v.y * v.y );
				if (vSize < FLT_EPSILON)
					DeleteObject(i);
			}
		}
	}*/
}

//디버그는 호출하는 놈을 찾아야 한다.	--> 호출 스택

void ScnMgr::KeyDownInput(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W')
	{
		m_key.notuse0 = true;
	}
	if (key == 'a' || key == 'A')
	{
		m_key.notuse1 = true;
	}
	if (key == 's' || key == 'S')
	{
		m_key.notuse2 = true;
	}
	if (key == 'd' || key == 'D')
	{
		m_key.notuse3 = true;
	}
	if (key == 'r' || key == 'R')
	{
		m_Player[0]->SetPos({ 0, 0 });
	}
}

void ScnMgr::KeyUpInput(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W')
	{
		m_key.notuse0 = false;
	}
	if (key == 'a' || key == 'A')
	{
		m_key.notuse1 = false;
	}
	if (key == 's' || key == 'S')
	{
		m_key.notuse2 = false;
	}
	if (key == 'd' || key == 'D')
	{
		m_key.notuse3 = false;
	}
}

void ScnMgr::SpecialKeyDownInput(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		m_key.Up = true;
	}
	if (key == GLUT_KEY_LEFT)
	{
		m_key.Left = true;
	}
	if (key == GLUT_KEY_DOWN)
	{
		m_key.Down = true;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		m_key.Right = true;
	}
}

void ScnMgr::SpecialKeyUpInput(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		m_key.Up = false;
	}
	if (key == GLUT_KEY_LEFT)
	{
		m_key.Left = false;
	}
	if (key == GLUT_KEY_DOWN)
	{
		m_key.Down = false;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		m_key.Right = false;
	}
}



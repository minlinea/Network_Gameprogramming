#include "stdafx.h"
#include "ScnMgr.h"
#include "Dependencies\freeglut.h"
#include <float.h>

int Title_arr[TitleSize][TitleSize] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,1,1,1,1,0,0,0,1,0,0,0,0,1,0,1,0,0,1,1,1,1,0,0},
		{0,0,1,0,0,0,0,0,1,0,1,0,0,1,1,1,1,1,0,1,0,0,0,0,0},
		{0,0,1,0,1,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,1,1,1,0,0},
		{0,0,1,0,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,0,0,0,0},
		{0,0,1,1,1,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,1,1,1,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
		{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
		{0,0,1,0,0,1,1,0,1,1,1,0,0,0,0,1,1,0,1,1,1,0,1,0,0},
		{0,0,1,0,1,0,0,0,0,1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0},
		{0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,1,1,0,0,1,0,0,1,0,0},
		{0,0,1,0,0,0,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,0,1,0,0},
		{0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,1,0,0},
		{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
		{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
		{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		
                                 //
};
ScnMgr::ScnMgr()
{

	// Initialize Renderer
	m_Renderer = new Renderer(WINDOW_WIDTH, WINDOW_HEIGH);		// 10x10m 방
	if (!m_Renderer->IsInitialized())
	{
		std::cout << "Renderer could not be initialized.. \n";
	}
	///////////////////Mathing////////////////////////////

	m_client = new TCPClient();

	CurrentScene = GameScene;

	////////////////////////////////////////////////////////////////
	//Initialize objects
	for (int i = 0; i < MAX_PLAYER_NUM; ++i) {
		m_Player[i] = new Object();
		m_Player[i]->SetPos({ 0,0 });
		m_Player[i]->SetVol({ 1,1 });
		if (i == 0)
			m_Player[i]->SetColor({ 1, 0, 0, 1 });
		else if (i == 1)
			m_Player[i]->SetColor({ 0, 1, 0, 1 });
		else if (i == 2)
			m_Player[i]->SetColor({ 0, 0, 1, 1 });
		m_Player[i]->SetVel({ 1,1 });

		m_Player[i]->SetType(Player);
	}
	int map_arr[MAP_COLUMN][MAP_ROW] = {
		{1,2,3,2,3,2,1,2,4,2,3,2,3,2,2,1},
		{1,2,2,3,3,3,1,1,1,3,3,3,2,2,1,1},
		{1,2,2,2,3,2,3,2,3,2,3,2,2,2,2,1},
		{3,3,3,3,2,3,3,3,3,3,2,3,3,3,3,3},
		{3,2,3,2,3,3,2,2,2,3,3,2,2,3,3,3},
		{3,2,3,1,3,2,2,2,2,2,1,1,1,3,3,3},
		{3,1,3,2,2,2,2,2,2,2,1,2,3,3,3,3},
		{3,2,3,1,3,3,2,2,2,2,1,1,1,3,2,3},
		{3,1,3,2,1,1,2,2,2,2,3,2,3,3,1,3},
		{3,2,3,3,2,1,2,2,2,2,2,3,3,3,2,3},
		{3,1,3,2,1,1,2,2,2,2,3,2,3,3,1,3},
		{3,2,2,3,3,3,2,3,2,3,3,3,2,3,2,3},
		{3,3,3,2,2,2,3,3,3,3,2,2,3,3,3,3},
		{1,2,3,3,3,3,3,2,3,2,3,3,2,3,2,1},
		{1,2,2,3,3,3,1,1,1,3,3,3,3,2,2,1},
		{1,2,2,2,3,2,1,2,1,2,3,2,2,2,2,1},
	};
	for (int i = 0; i < MAP_COLUMN; ++i)
	{
		for (int j = 0; j < MAP_ROW; ++j)
		{
			m_MapObject[i][j] = new Object();
			m_MapObject[i][j]->SetPos({ (float)(j - MAP_ROW / 2),(float)((MAP_COLUMN - i - 1) - MAP_COLUMN / 2) });
			m_MapObject[i][j]->SetVol({ 1,1 });
			m_MapObject[i][j]->SetVel({ 0,0 });

			m_MapObject[i][j]->SetColor({ 0, 0, 0, 1 });

			m_MapObject[i][j]->SetType(Empty);
			//(ObjectType)(map_arr[i][j])
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
	if (CurrentScene == TitleScene) {
		for (int i = 0; i < MAP_COLUMN; ++i)
		{
			float interval = 20;
			for (int j = 0; j < MAP_ROW; ++j)
			{
				m_Renderer->DrawSolidRect(
					j * interval + interval / 2, i * interval + interval / 2, 0,
					1 * interval, 0.35, 0.35, 0.5, 1);
			}
		}
	}
	else if (CurrentScene == GameScene) {
		/*********************************************PlayScene***************************************************************************/
		Float2 pos;
		Float2 vol;
		Color4 col;
		float interval = 50;
		ObjectType type;


		//draw marginal map wall
	/*	for (int i = -9; i < 9; ++i)
		{
			m_Renderer->DrawSolidRect(i * interval + interval * 0.5, -9 * interval + interval * 0.5, 0, interval, 1, 0, 1, 1);
			m_Renderer->DrawSolidRect(-9 * interval + interval * 0.5, i * interval + interval * 0.5, 0, interval, 1, 0, 1, 1);
			m_Renderer->DrawSolidRect(8 * interval + interval * 0.5, i * interval + interval * 0.5, 0, interval, 1, 0, 1, 1);
			m_Renderer->DrawSolidRect(i * interval + interval * 0.5, 8 * interval + interval * 0.5, 0, interval, 1, 0, 1, 1);
		}*/

		for (int i = 0; i < MAP_COLUMN; ++i)
		{
			for (int j = 0; j < MAP_ROW; ++j)
			{
				m_MapObject[i][j]->GetPos(&pos);
				m_MapObject[i][j]->GetVol(&vol);
				m_MapObject[i][j]->GetColor(&col);
				m_MapObject[i][j]->GetType(&type);
				int size = 0;

				switch (type)
				{
				case Wall:
					size = 50;
					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2, pos.y * interval + interval / 2, 0,
						vol.x * interval, 0.35, 0.35, 0.5, 1);
					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2, pos.y * interval + interval / 2, 0,
						vol.x * 30, 0.25, 0.25, 0.35, 1);
					break;
				case Breakable_Wall:
					size = 50;
					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2, pos.y * interval + interval / 2, 0,
						vol.x * size, 0.8, 0.7, 0.5, 1);
					break;
				case Item_AddSpeed:
					size = 20;
					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2, pos.y * interval + interval / 2, 0,
						vol.x * size, 1, 0.66, 0.35, 1);
					break;
				case Item_AddBomb:
					size = 20;
					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2, pos.y * interval + interval / 2, 0,
						vol.x * size, 1, 1, 1, 1);
					break;
				case Item_AddPower:
					size = 20;
					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2, pos.y * interval + interval / 2, 0,
						vol.x * size, 1, 0.65, 1, 1);
					break;
				case Bomb_Anim1:
					size = 35;

					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2, pos.y * interval + interval / 2, 0,
						vol.x * size, 0, 0.65, 0.9, 1);
					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2 + size / 3, pos.y * interval + interval / 2 + size / 4, 0,
						vol.x * size / 10, 0.9, 0.9, 0.9, 1);
					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2 + size / 3 - vol.x * size / 10, pos.y * interval + interval / 2 + size / 4, 0,
						vol.x * size / 10, 0.9, 0.9, 0.9, 1);
					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2 + size / 3, pos.y * interval + interval / 2 + size / 4 - vol.x * size / 10, 0,
						vol.x * size / 10, 0.9, 0.9, 0.9, 1);
					m_Renderer->DrawSolidRect(
						pos.x * interval + interval / 2, pos.y * interval + interval / 2 + size / 2, 0,
						vol.x * size / 4, 0, 0.5, 0.9, 1);
					break;
				}
			}
		}

		//draw character
		for (int i = 0; i < MAX_PLAYER_NUM; ++i) {
			m_Player[i]->GetPos(&pos);
			m_Player[i]->GetVol(&vol);
			m_Player[i]->GetColor(&col);
			float body_gap = interval / 4;
			//head
			m_Renderer->DrawSolidRect(pos.x * interval, pos.y * interval + body_gap * 2, 0, vol.x * body_gap, 1, 1, 1, 1);
			//body
			m_Renderer->DrawSolidRect(pos.x * interval, pos.y * interval, 0, vol.x * body_gap * 3, col.r, col.g, col.b, col.a);
		}

		for (int i = 0; i < TitleSize; ++i)
		{
			float interval = 1000/ TitleSize;
			for (int j = 0; j < TitleSize; ++j)
			{
				if (Title_arr[i][j] == 1) {
					m_Renderer->DrawSolidRect(
						(j- TitleSize/2) * interval, (TitleSize -1-i- TitleSize/2) * interval, 0,
						1 * interval, 0.35, 0.35, 0.5, 1);
				}
			}
		}
		/******************************************************************************************************************************/
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
	if (CurrentScene == TitleScene) {
		unsigned char msg;
		msg = Msg_Ready;
		std::cout << "update" << std::endl;
		m_client->TitleSceneSendData(msg);

		m_client->TitleSceneRecvData(msg);
		if (msg == Msg_ConfirmReadyCancel)
			delete m_client;
		else if (msg == Msg_PlayGame)
		{
			CurrentScene = GameScene;
		}
	}
	else if (CurrentScene == GameScene)
	{
		CharacterStatus pos[3];

		m_client->PlaySceneSendData(m_key);
		m_client->PlaySceneRecvData(pos);
		ObjectType type;
		for (int i = 0; i < MAX_PLAYER_NUM; ++i) {
			m_Player[i]->Update(pos[i], ElapsedTime);
		}
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
	if (CurrentScene == GameScene) {
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
}

void ScnMgr::KeyUpInput(unsigned char key, int x, int y)
{
	if (CurrentScene == GameScene) {
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
}

void ScnMgr::SpecialKeyDownInput(int key, int x, int y)
{
	if (CurrentScene == GameScene) {
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
}

void ScnMgr::SpecialKeyUpInput(int key, int x, int y)
{
	if (CurrentScene == GameScene) {
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
}



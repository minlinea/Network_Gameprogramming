#include "stdafx.h"
#include "ScnMgr.h"
#include "Dependencies\freeglut.h"
#include <float.h>


ScnMgr::ScnMgr()
{
	m_client = new TCPClient();
	// Initialize Renderer
	m_Renderer = new Renderer(1000, 1000);		// 10x10m 방
	if (!m_Renderer->IsInitialized())
	{
		std::cout << "Renderer could not be initialized.. \n";
	}

	//Initialize objects
	for (int i = 0; i < MAX_OBJ_COUNT; ++i)
	{
		m_Obj[i] = NULL;
	}
	//가급적 수동으로 지우는건 지양하지만, 초기화 할 때는 괜찮다.

	m_Obj[HERO_ID] = new Object();
	m_Obj[HERO_ID]->SetPos({ 0,0 });
	m_Obj[HERO_ID]->SetVol({ 1,1 });
	m_Obj[HERO_ID]->SetColor({ 1, 1, 1, 1 });
	m_Obj[HERO_ID]->SetVel({ 1,1 });

	m_Obj[HERO_ID]->SetType(TYPE_NORMAL);
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

	for (int i = 0; i < MAX_OBJ_COUNT; ++i)
	{
		if (m_Obj[i] != NULL)
		{
			Float2 pos;
			Float2 vol;
			Color4 col;
			m_Obj[i]->GetPos(&pos);
			m_Obj[i]->GetVol(&vol);
			m_Obj[i]->GetColor(&col);

			m_Renderer->DrawSolidRect(pos.x * 100.f, pos.y * 100.f, 0, vol.x * 100.f, col.r, col.g, col.b, col.a);
		}
	}
}

//int ScnMgr::AddObject(float x, float y, float z,
//					float sx, float sy, float sz,
//					float r, float g, float b, float a,
//					float vx, float vy, float vz,
//					float mass,
//					float fricCoef,
//					int type)
//{
//	// TEST OBJECT CLASS
//	int idx = -1;
//
//	for (int i = 0; i < MAX_OBJ_COUNT; ++i)
//	{
//		if (m_Obj[i] == NULL)
//		{
//			idx = i;
//			break;
//		}
//	}
//	if (idx == -1)
//	{
//		std::cout << "No more empty obj slot. " << std::endl;
//		return -1;
//	}
//
//	m_Obj[idx] = new Object();
//	m_Obj[idx]->SetPos(x, y, z);
//	m_Obj[idx]->SetVol(sx, sy, sz);
//	m_Obj[idx]->SetColor(r, g, b, a);
//	m_Obj[idx]->SetVel(vx, vy, vz);
//	m_Obj[idx]->SetMass(mass);
//	m_Obj[idx]->SetFricCoef(fricCoef);
//	m_Obj[idx]->SetType(type);
//	return idx;
//
//}

void ScnMgr::DeleteObject(int idx)
{
	if (idx < 0)
	{
		std::cout << "input idx is negative : " << idx << std::endl;
		return;
	}

	if (idx >= MAX_OBJ_COUNT)
	{
		std::cout << "input idx exceeds MAX_OBJ_COUNT : " << idx << std::endl;
		return;
	}

	if (m_Obj[idx] == NULL)
	{
		std::cout << "m_Obj[" << idx << "] is NULL" << std::endl;
		return;
	}

	delete m_Obj[idx];
	m_Obj[idx] = NULL;
	return;
}

Object* ScnMgr::GetObj() const
{
	return *m_Obj;
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

	for (int i = 0; i < MAX_OBJ_COUNT; ++i)
	{
		if (m_Obj[i] != NULL)
			m_Obj[i]->Update(pos[0],ElapsedTime);
	}
}

void ScnMgr::DoGarbageCollection()
{
	for (int i = 0; i < MAX_OBJ_COUNT; ++i)
	{
		if (m_Obj[i] != NULL)
		{
			int type;
			m_Obj[i]->GetType(&type);
			if (type == TYPE_BULLET)
			{
				Float2 v;
				m_Obj[i]->GetVel(&v);
				float vSize = sqrtf(v.x * v.x + v.y * v.y );
				if (vSize < FLT_EPSILON)
					DeleteObject(i);
			}
		}
	}
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
		m_Obj[HERO_ID]->SetPos({ 0, 0 });
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



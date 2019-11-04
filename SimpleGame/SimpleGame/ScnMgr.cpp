#include "stdafx.h"
#include "ScnMgr.h"
#include "Dependencies\freeglut.h"
#include <float.h>


ScnMgr::ScnMgr()
{
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
	m_Obj[HERO_ID]->SetPos(0,0,0);
	m_Obj[HERO_ID]->SetVol(0.5,0.5,0.5);
	m_Obj[HERO_ID]->SetColor(1, 1, 1, 1);
	m_Obj[HERO_ID]->SetVel(0, 0, 0);
	m_Obj[HERO_ID]->SetMass(1);	//1kg
	m_Obj[HERO_ID]->SetFricCoef(0.7);
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

	// Renderer Test
	//m_Renderer->DrawSolidRect(50, 0, 0, 50, 0, 0, 1, 1);
	//m_Renderer->DrawSolidRect(0, 0, 0, 100, 1, 1, 0, 1);

	for (int i = 0; i < MAX_OBJ_COUNT; ++i)
	{
		if (m_Obj[i] != NULL)
		{
			float x, y, z = 0;
			float sx, sy, sz = 0;
			float r, g, b, a = 0;
			m_Obj[i]->GetPos(&x, &y, &z);
			m_Obj[i]->GetVol(&sx, &sy, &sz);
			m_Obj[i]->GetColor(&r, &g, &b, &a);

			//1m = 100cm = 100pixels
			//x = x * 100.f;	sx = sx * 100.f;
			//y = y * 100.f;	 sy = sy * 100.f;
			//z = z * 100.f;	 sz = sz * 100.f;
			m_Renderer->DrawSolidRect(x * 100.f, y * 100.f, z * 100.f, sx * 100.f, r, g, b, a);
		}
		//else
		//{
		//	std::cout << "m_Obj[" << i << "] is NULL" << std::endl;
		//}
	}
}

int ScnMgr::AddObject(float x, float y, float z,
					float sx, float sy, float sz,
					float r, float g, float b, float a,
					float vx, float vy, float vz,
					float mass,
					float fricCoef,
					int type)
{
	// TEST OBJECT CLASS
	int idx = -1;

	for (int i = 0; i < MAX_OBJ_COUNT; ++i)
	{
		if (m_Obj[i] == NULL)
		{
			idx = i;
			break;
		}
	}
	if (idx == -1)
	{
		std::cout << "No more empty obj slot. " << std::endl;
		return -1;
	}

	m_Obj[idx] = new Object();
	m_Obj[idx]->SetPos(x, y, z);
	m_Obj[idx]->SetVol(sx, sy, sz);
	m_Obj[idx]->SetColor(r, g, b, a);
	m_Obj[idx]->SetVel(vx, vy, vz);
	m_Obj[idx]->SetMass(mass);
	m_Obj[idx]->SetFricCoef(fricCoef);
	m_Obj[idx]->SetType(type);
	return idx;

}

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

Object *ScnMgr::GetObj() const
{
	return *m_Obj;
}

void ScnMgr::Update(float ElapsedTime)
{
	//character control
	//std::cout << "W: " << m_KeyW
	//			<< ", A: " << m_KeyA
	//			<< ", S: " << m_KeyS
	//			<< ", D: " << m_KeyD << std::endl;

		//std::cout << "Up: " << m_KeyUp
		//<< ", Left: " << m_KeyLeft
		//<< ", Down: " << m_KeyDown
		//<< ", Right: " << m_KeyRight << std::endl;

	float fx, fy, fz;
	fx = fy = fz = 0.f;
	float fAmount = 10.f;
	if (m_KeyW)
	{
		fy += 1.f;
	}
	if (m_KeyS)
	{
		fy -= 1.f;
	}
	if (m_KeyD)
	{
		fx += 1.f;
	}
	if (m_KeyA)
	{
		fx -= 1.f;
	}

	// Add control force to hero
	float fsize = sqrtf(fx*fx + fy * fy + fz * fz);
	if (fsize > FLT_EPSILON)
	{
		fx /= fsize;
		fy /= fsize;
		fz /= fsize;
		fx *= fAmount;
		fy *= fAmount;
		fz *= fAmount;
		m_Obj[HERO_ID]->AddForce(fx, fy, fz, ElapsedTime);
	}
	//Fire bullets
	if(m_Obj[HERO_ID]->CanShootBullet())
	{
		float bulletVel = 5.f;
		float vBulletX, vBulletY, vBulletZ;
		vBulletX = vBulletY = vBulletZ = 0.f;

		if (m_KeyUp)
			vBulletY += 1.f;
		if (m_KeyLeft)
			vBulletX -= 1.f;
		if (m_KeyRight)
			vBulletX += 1.f;
		if (m_KeyDown)
			vBulletY -= 1.f;

		float vBulletSize = sqrtf(vBulletX * vBulletX + vBulletY * vBulletY + vBulletZ * vBulletZ);
		if (vBulletSize > FLT_EPSILON)
		{
			//create bullet
			//normalize
			vBulletX /= vBulletSize;
			vBulletY /= vBulletSize;
			vBulletZ /= vBulletSize;
			//실제 속도
			vBulletX *= bulletVel;
			vBulletY *= bulletVel;
			vBulletZ *= bulletVel;

			float hx, hy, hz;
			float hvx, hvy, hvz;
			m_Obj[HERO_ID]->GetPos(&hx, &hy, &hz);
			m_Obj[HERO_ID]->GetVel(&hvx, &hvy, &hvz);

			vBulletX += hvx;
			vBulletY += hvy;
			vBulletZ += hvz;

			AddObject(hx, hy, hz,
							0.1, 0.1, 0.1,
							1, 0, 0, 1,
							vBulletX, vBulletY, vBulletZ,
							1.f,
							0.7f, 
							TYPE_BULLET);

			m_Obj[HERO_ID]->ResetBulletCoolTime();
		}
	}




	for (int i = 0; i < MAX_OBJ_COUNT; ++i)
	{
		if (m_Obj[i] != NULL)
			m_Obj[i]->Update(ElapsedTime);
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
				float vx, vy, vz;
				m_Obj[i]->GetVel(&vx, &vy, &vz);
				float vSize = sqrtf(vx * vx + vy * vy + vz * vz);
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
		m_KeyW = true;
	}
	if (key == 'a' || key == 'A')
	{
		m_KeyA = true;
	}
	if (key == 's' || key == 'S')
	{
		m_KeyS = true;
	}
	if (key == 'd' || key == 'D')
	{
		m_KeyD = true;
	}
	if (key == 'r' || key == 'R')
	{
		m_Obj[HERO_ID]->SetPos(0, 0, 0);
	}
}

void ScnMgr::KeyUpInput(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W')
	{
		m_KeyW = false;
	}
	if (key == 'a' || key == 'A')
	{
		m_KeyA = false;
	}
	if (key == 's' || key == 'S')
	{
		m_KeyS = false;
	}
	if (key == 'd' || key == 'D')
	{
		m_KeyD = false;
	}
}

void ScnMgr::SpecialKeyDownInput(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		m_KeyUp = true;
	}
	if (key == GLUT_KEY_LEFT)
	{
		m_KeyLeft = true;
	}
	if (key == GLUT_KEY_DOWN)
	{
		m_KeyDown = true;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		m_KeyRight = true;
	}
}

void ScnMgr::SpecialKeyUpInput(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		m_KeyUp = false;
	}
	if (key == GLUT_KEY_LEFT)
	{
		m_KeyLeft = false;
	}
	if (key == GLUT_KEY_DOWN)
	{
		m_KeyDown = false;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		m_KeyRight = false;
	}
}



#include "stdafx.h"
#include "ScnMgr.h"
#include "Renderer.h"
#include "Dependencies\freeglut.h"
#include <float.h>

ScnMgr::ScnMgr() 
{
	// Initialize Renderer
	m_Renderer = new Renderer(500, 500);	//500x500 픽셀   5미터

	if (!m_Renderer->IsInitialized())
	{
		std::cout << "Renderer could not be initialized.. \n";
	}

	//Initialize Objects
	for (int i = 0; i < MAX_OBJ_NUM; ++i)
	{
		m_Object[i] = NULL;
	}

	m_Object[HERO_ID] = new Object;
	m_Object[HERO_ID]->SetPos(0, 0, 0);
	m_Object[HERO_ID]->SetVol(0.5, 0.5, 0.5);
	m_Object[HERO_ID]->SetColor(0, 1, 0, 1);
	m_Object[HERO_ID]->SetVel(0, 0, 0);
	m_Object[HERO_ID]->SetMass(1);
	m_Object[HERO_ID]->SetFricCoef(0.7);
	m_Object[HERO_ID]->SetType(TYPE_NORMAL);

}
ScnMgr::~ScnMgr()
{
	if (m_Renderer != NULL)
	{
		delete m_Renderer;
		m_Renderer = NULL;
	}
}

void ScnMgr::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	
	float x, y, z = 0;
	float sx, sy, sz = 0;
	float r,g,b,a = (rand()%10)/100;
	
	//Draw all m_Objs
	for (int i = 0; i < MAX_OBJ_NUM; ++i)
	{
		if (m_Object[i] != NULL)
		{
			m_Object[i]->GetPos(&x, &y, &z);
			m_Object[i]->GetVol(&sx, &sy, &sz);
			m_Object[i]->GetColor(&r, &g, &b, &a);
			//1 meter ==100 cm == 100 pixels
			x = x * 100.f;
			y = y * 100.f;
			z = z * 100.f;
			sx = sx * 100.f;
			sy = sy * 100.f;
			sz = sz * 100.f;

			m_Renderer->DrawSolidRect(x, y, z, sx, r, g, b, a);
		}
		
	}
}

void ScnMgr::DoGarbageCollection()
{
	for (int i = 0; i < MAX_OBJ_NUM; ++i)
	{
		if (m_Object[i] == NULL)continue;
		int type = -1;
		m_Object[i]->GetType(&type);
		if (type == TYPE_BULLET)
		{
			//Check velocity size
			float vx, vy, vz;
			m_Object[i]->GetVel(&vx, &vy, &vz);
			float vSize = sqrtf(vx * vx + vy * vy + vz * vz);
			if (vSize < FLT_EPSILON)
			{
				DeleteObject(i);
			}
		}
	}
}

int ScnMgr::AddObj(float x, float y, float z,
	float sx, float sy, float sz,
	float r, float g, float b, float a,
	float vx, float vy, float vz,
	float mass, float fricCoef, int type)
{
	int idx = -1;
	for (int i = 0; i < MAX_OBJ_NUM; ++i)
	{
		if (m_Object[i] == NULL)
		{
			idx = i;
			break;
		}
	}
	if (idx == -1)
	{
		std::cout << "No more empty obj slot." << std::endl;
		return -1;
	}

	m_Object[idx] = new Object;
	m_Object[idx]->SetPos(x,y,z);
	m_Object[idx]->SetVol(sx,sy,sz);
	m_Object[idx]->SetColor(r, g, b, a);
	m_Object[idx]->SetVel(vx,vy,vz);
	m_Object[idx]->SetMass(mass);
	m_Object[idx]->SetFricCoef(fricCoef);
	m_Object[idx]->SetType(type);
}

void ScnMgr::DeleteObject(int idx)
{
	if (idx < 0)
	{
		std::cout << "input idx is nagative :" << idx << std::endl;
		return;
	}
	if (idx >= MAX_OBJ_NUM)
	{
		std::cout << "input idx exceeds MAX_OBJ_NUM :" << idx << std::endl;
		return;
	}

	if (m_Object[idx] == NULL)
	{
		std::cout << "m_Object["<<idx<<"] is NULL :" <<  std::endl;
		return;
	}

	delete m_Object[idx];
	m_Object[idx] = NULL;
}

void ScnMgr::Update(float elapsedTimeInSec)
{
	//Character Control : Hero
	//std::cout << "W: " << m_KeyW << ", A: " << m_KeyA << ", S: " << m_KeyS << ", D: " << m_KeyD << std::endl;
	//std::cout << "Up: " << m_KeyUp << ", Left: " << m_KeyLeft << ", Down: " << m_KeyDown << ", Right: " << m_KeyRight << std::endl;
	float fx, fy, fz;
	fx = fy = fz = 0;
	float fAmount = 9; //1뉴턴
	if (m_KeyW)
	{
		fy += 1.f * fAmount;
	}
	if (m_KeyA)
	{
		fx -= 1.f * fAmount;
	}
	if (m_KeyS)
	{
		fy -= 1.f * fAmount;
	}
	if (m_KeyD)
	{
		fx += 1.f * fAmount;
	}
	//Add control force to Hero
	m_Object[HERO_ID]->AddForce(fx, fy, fz, elapsedTimeInSec);

	//Fire bullets
	if (m_KeyUp | m_KeyDown | m_KeyLeft | m_KeyRight)
	{
		float bulletVel = 5.f;
		float vBulletX, vBulletY, vBulletZ;
		vBulletX= vBulletY= vBulletZ=0.f;
		if (m_KeyUp) vBulletY += 1.f;
		if (m_KeyDown) vBulletY -= 1.f;
		if (m_KeyLeft) vBulletX -= 1.f;
		if (m_KeyRight) vBulletX += 1.f;

		float vBulletSize = sqrtf(vBulletX * vBulletX + vBulletY * vBulletY + vBulletZ * vBulletZ);

		if (vBulletSize > FLT_EPSILON)
		{
			//Create bullet object
			vBulletX /= vBulletSize;
			vBulletY /= vBulletSize;
			vBulletZ /= vBulletSize;
			vBulletX *= bulletVel;
			vBulletY *= bulletVel;
			vBulletZ *= bulletVel;

			float hx, hy, hz;
			float hvx, hvy, hvz;
			m_Object[HERO_ID]->GetPos(&hx, &hy, &hz);
			m_Object[HERO_ID]->GetVel(&hvx, &hvy, &hvz);

			vBulletX += hvx;
			vBulletY += hvy;
			vBulletZ += hvz;

			AddObj(hx, hy, hz,
				0.05f, 0.05f, 0.05f,
				1, 0, 0, 1,
				vBulletX, vBulletY, vBulletZ,
				1.f,
				0.7f,
				TYPE_BULLET);
		}
	}

	for (int i = 0; i < MAX_OBJ_NUM; ++i)
	{
		if (m_Object[i] != NULL)m_Object[i]->Update(elapsedTimeInSec);
	}

	if (m_KeyE)
	{
		m_ServerData.Down = m_KeyS;
		m_ServerData.Up = m_KeyW;
		m_ServerData.Left = m_KeyA;
		m_ServerData.Right = m_KeyD;
		m_ServerData.Interact1 = 0;
		m_ServerData.Interact2 = 0;
		m_ServerData.Interact3 = 0;
		m_ServerData.Interact4 = 0;


		m_client->SendData(&m_ServerData);
		m_client->RecvData(&m_ServerData);
		//std::cout << m_ServerData.Up << m_ServerData.Down << m_ServerData.Left << m_ServerData.Right << m_ServerData.Interact1 << m_ServerData.Interact2 << m_ServerData.Interact3 << m_ServerData.Interact4 << std::endl;
	}
}

void ScnMgr::KeyDownInput(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W')
	{
		m_KeyW = true;
	}
	if (key == 's' || key == 'S')
	{
		m_KeyS = true;
	}
	if (key == 'a' || key == 'A')
	{
		m_KeyA = true;
	}
	if (key == 'd' || key == 'D')
	{
		m_KeyD = true;
	}
	if (key == 'e' && m_KeyE == false)
	{
		m_client = new TCPClientClass();
		m_KeyE = true;
	}
}
void ScnMgr::KeyUpInput(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W')
	{
		m_KeyW = false;
	}
	if (key == 's' || key == 'S')
	{
		m_KeyS = false;
	}
	if (key == 'a' || key == 'A')
	{
		m_KeyA = false;
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
	if (key == GLUT_KEY_RIGHT)
	{
		m_KeyRight = true;
	}
	if (key == GLUT_KEY_DOWN)
	{
		m_KeyDown = true;
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
	if (key == GLUT_KEY_RIGHT)
	{
		m_KeyRight = false;
	}
	if (key == GLUT_KEY_DOWN)
	{
		m_KeyDown = false;
	}
}

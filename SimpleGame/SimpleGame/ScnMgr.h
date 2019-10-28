#pragma once
#include "Globals.h"
#include "Renderer.h"
#include "Object.h"
#include "TCPClientClass.h"

class ScnMgr
{
private:
	Renderer* m_Renderer = NULL; //동적할당 NULL로 초기화 해놓자
	Object* m_Object[MAX_OBJ_NUM];
	TCPClientClass* m_client;
	InputData m_ServerData;

	bool m_KeyW = false;
	bool m_KeyA = false;
	bool m_KeyS = false;
	bool m_KeyD = false;

	bool m_KeyUp = false;
	bool m_KeyLeft = false;
	bool m_KeyDown = false;
	bool m_KeyRight = false;

	bool m_KeyE = false;

public:
	ScnMgr();
	~ScnMgr();

public:
	void Update(float elapsedTimeInSec);
	void RenderScene();
	void DoGarbageCollection();
	int AddObj(float x, float y, float z,
		float sx,float sy, float sz,
		float r, float g, float b, float a,
		float vx,float vy,float vz,
		float mass,
		float fricCoef,
		int type);
	float m_velX, m_velY, m_velZ;	//velocity
	float m_mass;					//mass
	float m_fricCoef;				//friction coeff
	int m_type;

	void DeleteObject(int idx);

	void KeyDownInput(unsigned char key, int x, int y);
	void KeyUpInput(unsigned char key, int x, int y);
	void SpecialKeyDownInput(int key, int x, int y);
	void SpecialKeyUpInput(int key, int x, int y);

};


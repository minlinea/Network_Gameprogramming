#pragma once
#include "Renderer.h"
#include "Object.h"
#include "Globals.h"


class ScnMgr
{
public:
	ScnMgr();
	~ScnMgr();

	
	int AddObject(float x, float y, float z,
					float sx, float sy, float sz,
					float r, float g, float b, float a,
					float vx, float vy, float vz,
					float mass,
					float fricCoef,
					int type
	);
					

	Object *GetObj() const; 
	void DeleteObject(int idx);

	void RenderScene(float ElapsedTime);
	void Update(float ElapsedTime);
	void DoGarbageCollection();

	void KeyDownInput(unsigned char key, int x, int y);
	void KeyUpInput(unsigned char key, int x, int y);
	void SpecialKeyDownInput(int key, int x, int y);
	void SpecialKeyUpInput(int key, int x, int y);

private:
	Renderer *m_Renderer{ NULL };
	Object * m_Obj[MAX_OBJ_COUNT];

	bool m_KeyW = false;
	bool m_KeyA = false;
	bool m_KeyS = false;
	bool m_KeyD = false;

	bool m_KeyUp = false;
	bool m_KeyLeft = false;
	bool m_KeyDown = false;
	bool m_KeyRight = false;
};


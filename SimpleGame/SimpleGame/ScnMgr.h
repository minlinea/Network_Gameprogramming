#pragma once
#include "Renderer.h"
#include "Object.h"
#include "Globals.h"
#include "TCPClient.h"

class ScnMgr
{
public:
	ScnMgr();
	~ScnMgr();

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

	KeyInput m_key;

	TCPClient* m_client;
};


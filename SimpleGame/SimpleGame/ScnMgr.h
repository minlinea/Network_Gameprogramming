#pragma once
#include "Renderer.h"
#include "Object.h"
#include "Globals.h"
#include "TCPClient.h"
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"
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
	Object* m_Player[MAX_PLAYER_NUM];
	Object * m_MapObject[MAP_COLUMN][MAP_ROW];

	KeyInput m_key;

	TCPClient* m_client;
	enum Scene
	{
		TitleScene,
		GameScene,
		EndingScene
	};
	Scene CurrentScene;
	int CurrentButton;
	bool isPressed;
};


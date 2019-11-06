/*
Copyright 2017 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/


/*
ScnMgr 클래스 구현
겜소공 수업에서 ScnMgr은 향후 추가될 모든 클래스의 기반이 되는 클래스
모든 클래스가 이 안의 리스트 함수로 들어가는 식으로 구현

메모리 관리 철저
*/








#include "stdafx.h"
#include <iostream>
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"

#include "ScnMgr.h"

ScnMgr* g_ScnMgr = NULL;
int g_PrevTime = 0;

void RenderScene(int temp)
{
	int CurrTime = glutGet(GLUT_ELAPSED_TIME);
	int ElapsedTime = CurrTime - g_PrevTime;
	float ElapsedTimeInSec = (float)ElapsedTime / 1000.f;
	//std::cout << ElapsedTimeInSec << std::endl;
	g_PrevTime = CurrTime;

	g_ScnMgr->Update(ElapsedTimeInSec);
	g_ScnMgr->RenderScene(ElapsedTimeInSec);
	g_ScnMgr->DoGarbageCollection();
	glutSwapBuffers();

	glutTimerFunc(10, RenderScene, 0);	// 10milliseconds에 RenderScene 호출
}

void Display(void)
{
}

void Idle(void)
{
}

void MouseInput(int button, int state, int x, int y)
{
}

void KeyDownInput(unsigned char key, int x, int y)
{
	g_ScnMgr->KeyDownInput(key, x, y);
}

void KeyUpInput(unsigned char key, int x, int y)
{
	g_ScnMgr->KeyUpInput(key, x, y);
}

void SpecialKeyDownInput(int key, int x, int y)
{
	g_ScnMgr->SpecialKeyDownInput(key, x, y);
}

void SpecialKeyUpInput(int key, int x, int y)
{
	g_ScnMgr->SpecialKeyUpInput(key, x, y);
}

int main(int argc, char** argv)
{
	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(400, 100);
	glutInitWindowSize(800, 800);		// 프로그램 외부 사이즈 결정
	glutCreateWindow("Game Software Engineering KPU");

	glewInit();
	if (glewIsSupported("GL_VERSION_3_0"))
	{
		std::cout << " GLEW Version is 3.0\n ";
	}
	else
	{
		std::cout << "GLEW 3.0 not supported\n ";
	}

	// 생성자는 이곳에

	g_ScnMgr = new ScnMgr;


	// 생성자는 이곳에

	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyDownInput);	// keydown event callback
	glutKeyboardUpFunc(KeyUpInput);	// keyup event callback
	glutMouseFunc(MouseInput);
	glutSpecialFunc(SpecialKeyDownInput);
	glutSpecialUpFunc(SpecialKeyUpInput);
	//초기화는 이곳에

	g_PrevTime = glutGet(GLUT_ELAPSED_TIME);
	glutTimerFunc(10, RenderScene, 0);	// 10milliseconds에 RenderScene 호출
	//초기화는 이곳에


	glutMainLoop();

	//소멸자는 이곳에
   //Object* p_Obj = g_ScnMgr->GetObj();
   //for (int i = 0; i < MAX_OBJ_COUNT; ++i)
   //{
   //	if (p_Obj[i] != NULL)
   //	{
   //		DeleteObject(i);
   //		std::cout << i << "번 오브젝트 삭제 완료" << std::endl;
   //	}
   //}

	delete g_ScnMgr;


	// 소멸자는 이곳에
	return 0;
}


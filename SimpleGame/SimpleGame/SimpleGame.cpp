/*
Copyright 2017 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/

#include "stdafx.h"
#include <iostream>
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"

#include "ScnMgr.h"


ScnMgr* g_ScnMgr = NULL;
float g_PrevTime;

void RenderScene(int temp)
{
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int elapsedTime = currTime - g_PrevTime;
	float elapsedTimeInSec = (float)elapsedTime / 1000.0f;
	g_PrevTime = currTime;

//std::cout << "elapsed time: " << elapsedTime << std::endl;
	g_ScnMgr->Update(elapsedTimeInSec);
	g_ScnMgr->RenderScene();
	g_ScnMgr->DoGarbageCollection();
	glutSwapBuffers();

	glutTimerFunc(10, RenderScene, 0);

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
int main(int argc, char **argv)
{
	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(500, 500);
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

	//init 밑에 있어야 함

	g_ScnMgr = new ScnMgr();

	glutDisplayFunc(Display);
	glutIdleFunc(Idle);

	glutKeyboardFunc(KeyDownInput);
	glutKeyboardUpFunc(KeyUpInput);

	glutMouseFunc(MouseInput);
	glutSpecialFunc(SpecialKeyDownInput);
	glutSpecialUpFunc(SpecialKeyUpInput);

	g_PrevTime = glutGet(GLUT_ELAPSED_TIME);
	glutTimerFunc(10, RenderScene, 0);

	glutMainLoop();

    return 0;
}


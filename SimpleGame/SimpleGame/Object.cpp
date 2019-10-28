#include "stdafx.h"
#include "Object.h"
#include <iostream>
#include <math.h>


Object::Object()
{
	InitPhysics();
}

Object::~Object()
{
}
/////////////////////////////////////////////////////////////////////
void Object::InitPhysics()
{
	m_posX = m_posY = m_posZ = 0;
	m_volX = m_volY = m_volZ = 0;
	m_velX = 0;
	m_velY = 0;
	m_velZ = 0;
	m_accX = m_accY = m_accZ = 0;
	m_r = m_g = m_b = m_a = 0;
	m_mass = 0;
	m_fricCoef = 0.f;
}
void Object::Update(float elapsedTimeInSec)
{
	/////////////Apply friction///////////////////
	float nForce = m_mass * GRAVITY;  //scalar
	float fForce = m_fricCoef * nForce;	//scalar
	float velSize = sqrtf(m_velX * m_velX + m_velY * m_velY);
	if (velSize > 0.f)
	{
		float fDirX = -1.f * m_velX / velSize;
		float fDirY = -1.f * m_velY / velSize;
		float fDirZ = -1.f * m_velZ / velSize;
		fDirX = fDirX * fForce;
		fDirY = fDirY * fForce;
		float fAccX = fDirX / m_mass;
		float fAccY = fDirY / m_mass;
		float newX = m_velX + fAccX * elapsedTimeInSec;
		float newY = m_velY + fAccY * elapsedTimeInSec;

		if (newX * m_velX < 0.f)
		{
			m_velX = 0.f;
		}
		else
			m_velX = newX;
		if (newY * m_velY < 0.f)
		{
			m_velY = 0.f;
		}
		else
			m_velY = newY;
	}
	
	//////////////////////////////////////////////
	m_posX += m_velX * elapsedTimeInSec;
	if (m_posX > WINWIDTH / 2)
		m_posX = -WINWIDTH / 2;
	if( m_posX < -WINWIDTH / 2)
		m_posX = WINWIDTH / 2;
	//std::cout << m_posX << std::endl;
	if (m_posY < WINHEIGHT / 2 && m_posY > -WINHEIGHT / 2)m_posY += m_velY * elapsedTimeInSec;
	else {
		m_posY -= m_velY * elapsedTimeInSec; m_velY *= -1;
	}
	if (m_posZ<250 && m_posZ > -250)m_posZ += m_velZ * elapsedTimeInSec;
	else {
		m_posZ -= m_velZ * elapsedTimeInSec; m_velZ *= -1;
	}

}

void Object::AddForce(float x, float y, float z,float elapsedTimeInSec)
{
	float accX, accY, accZ;
	accX = accY = accZ = 0.f;

	accX = x / m_mass;
	accY = y / m_mass;
	accZ = z / m_mass;

	m_velX = m_velX + accX * elapsedTimeInSec;
	m_velY = m_velY + accY * elapsedTimeInSec;
	m_velZ = m_velZ+ accZ * elapsedTimeInSec;
}

//////////////////////////////////////////////
float Object::GetPos(float* x, float* y, float* z)
{
	*x = m_posX;
	*y = m_posY;
	*z = m_posZ;
}

float Object::GetMass(float* mass)
{
	*mass = m_mass;
}
float Object::GetVel(float* x, float* y, float* z)
{
	*x = m_velX;
	*y = m_velY;
	*z = m_velZ;
}
float Object::GetAcc(float* x, float* y, float* z)
{
	*x = m_accX;
	*y = m_accY;
	*z = m_accZ;
}
float Object::GetVol(float* x, float* y, float* z)
{
	*x = m_volX;
	*y = m_volY;
	*z = m_volZ;
}


float Object::GetColor(float* r, float* g, float* b, float* a)
{
	*r = m_r;
	*g = m_g;
	*b = m_b;
	*a = m_a;
}


void Object::SetPos(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;
}

void Object::SetAcc(float x, float y, float z)
{
	m_accX = x;
	m_accY = y;
	m_accZ = z;
}
void Object::SetMass(float mass)
{
	m_mass = mass;
}
void Object::SetVel(float x, float y, float z)
{
	m_velX = x;
	m_velY = y;
	m_velZ = z;
}
void Object::SetVol(float x, float y, float z)
{
	m_volX = x;
	m_volY = y;
	m_volZ = z;
}
void Object::SetColor(float r, float g, float b, float a)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}
void Object::SetFricCoef(float coef)
{
	m_fricCoef = coef;
}
void Object::GetFricCoef(float *coef)
{
	*coef = m_fricCoef;
}

void  Object::SetType(int type)
{
	m_type = type;
}
void  Object::GetType(int* type)
{
	*type = m_type;
}
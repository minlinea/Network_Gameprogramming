#include "stdafx.h"
#include "Object.h"
#include "Globals.h"
#include <math.h>
#include <float.h>

Object::Object()
{
	InitPhysics();
}

Object::~Object()
{

}

void Object::Update(float ElapsedTime)
{
	// 쿨타임 줄이기
	m_RemainingCoolTime -= ElapsedTime;


	//마찰 계수 적용
	float nForce = m_mass * GRAVITY; //scalar
	float fForce = m_fricCoef * nForce;	//scalar
	float velSize = sqrtf(m_velX * m_velX + m_velY * m_velY);
	if (velSize > 0.f)
	{
		float fDirX = -1.f * m_velX / velSize;
		float fDirY = -1.f * m_velY / velSize;
		fDirX = fDirX * fForce;
		fDirY = fDirY * fForce;
		float fAccX = fDirX / m_mass;
		float fAccY = fDirY / m_mass;
		float newX = m_velX + fAccX * ElapsedTime;
		float newY = m_velY + fAccY * ElapsedTime;
		if (newX * m_velX < 0.f)
		{
			m_velX = 0.f;
		}
		else
		{
			m_velX = newX;
		}
		if (newY * m_velY < 0.f)
		{
			m_velY = 0.f;
		}
		else
		{
			m_velY = newY;
		}
	}
	//마찰 계수 적용


	//위치
	m_posX = m_posX + m_velX * ElapsedTime;
	m_posY = m_posY + m_velY * ElapsedTime;
	m_posZ = m_posZ + m_velZ * ElapsedTime;
}

void Object::AddForce(float x, float y, float z, float ElapsedTime)
{
	float accX, accY, accZ;
	accX = accY = accZ = 0.f;

	accX = x / m_mass;
	accY = y / m_mass;
	accZ = z / m_mass;

	m_velX = m_velX + accX * ElapsedTime;
	m_velY = m_velY + accY * ElapsedTime;
	m_velZ = m_velZ + accZ * ElapsedTime;
}

void Object::InitPhysics()
{
	float default_setting = 0;

	m_posX = m_posY = m_posZ = default_setting;
	m_r = m_g = m_b = m_a = default_setting;
	m_posX = m_posY = m_posZ = default_setting;

	m_velX = m_velY = m_velZ = default_setting;
	m_accX = m_accY = m_accZ = default_setting;
	m_volX = m_volY = m_volZ = default_setting;
	m_mass = default_setting;
	m_fricCoef = default_setting;
}

bool Object::CanShootBullet()
{
	if (m_RemainingCoolTime < FLT_EPSILON)
		return true;

	return false;
}
void Object::ResetBulletCoolTime()
{
	m_RemainingCoolTime = m_CurrentCoolTime;
}



void Object::SetColor(float r, float g, float b, float a)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}
void Object::GetColor(float* r, float* g, float* b, float* a)
{
	*r = m_r;
	*g = m_g;
	*b = m_b;
	*a = m_a;
}

void Object::SetPos(float posX, float posY, float posZ)
{
	m_posX = posX;
	m_posY = posY;
	m_posZ = posZ;
}
void Object::GetPos(float* posX, float* posY, float* posZ)
{
	*posX = m_posX;
	*posY = m_posY;
	*posZ = m_posZ;
}

void Object::SetMass(float mass)
{
	m_mass = mass;
}
void Object::GetMass(float* mass)
{
	*mass = m_mass;
}

void Object::SetAcc(float accX, float accY, float accZ)
{
	m_accX = accX;
	m_accY = accY;
	m_accZ = accZ;
}
void Object::GetAcc(float* accX, float* accY, float* accZ)
{
	*accX = m_accX;
	*accY = m_accY;
	*accZ = m_accZ;
}

void Object::SetVel(float velX, float velY, float velZ)
{
	m_velX = velX;
	m_velY = velY;
	m_velZ = velZ;
}
void Object::GetVel(float* velX, float* velY, float* velZ)
{
	*velX = m_velX;
	*velY = m_velY;
	*velZ = m_velZ;
}

void Object::SetVol(float volX, float volY, float volZ)
{
	m_volX = volX;
	m_volY = volY;
	m_volZ = volZ;
}
void Object::GetVol(float* volX, float* volY, float* volZ)
{
	*volX = m_volX;
	*volY = m_volY;
	*volZ = m_volZ;
}

void Object::SetFricCoef(float coef)
{
	m_fricCoef = coef;
}
void Object::GetFricCoef(float* coef)
{
	*coef = m_fricCoef;
}

void Object::SetType(int type)
{
	m_type = type;
}
void Object::GetType(int* type)
{
	*type = m_type;
}
#include "stdafx.h"
#include "Object.h"
#include <math.h>
#include <float.h>

Object::Object()
{
	//InitPhysics();
}

Object::~Object()
{

}

void Object::Update(CharacterStatus pos,float ElapsedTime)
{
	//��ġ
	m_position.x = pos.position.x;
	m_position.y = pos.position.y;
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
void Object::AddForce(float x, float y, float ElapsedTime)
{
	m_velocity.x += x;
	m_velocity.y += y;
}

void Object::SetColor(Color4 color)
{
	m_color = color;
}
void Object::GetColor(Color4* color)
{
	*color = m_color;
}
void Object::SetPos(Float2 position)
{
	m_position = position;
}
void Object::GetPos(Float2* position)
{
	*position = m_position;
}
void Object::SetVel(Float2 vel)
{
	m_velocity = vel;
}
void Object::GetVel(Float2* vel)
{
	*vel = m_velocity;
}
void Object::SetVol(Float2 volume)
{
	m_volume = volume;
}
void Object::GetVol(Float2* volume)
{
	*volume = m_volume;
}

void Object::SetType(int type)
{
	m_type = type;
}
void Object::GetType(int *type)
{
	*type = m_type;
}
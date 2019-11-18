#pragma once
#include "Globals.h"

class Object
{
	
public:
	Object();
	~Object();

	void Update(CharacterStatus pos, float ElapsedTime);

	void SetColor(Color4 color);
	void GetColor(Color4 *color);
	void SetPos(Float2 position);
	void GetPos(Float2 *position);
	void SetVel(Float2 vel);
	void GetVel(Float2*vel);
	void SetVol(Float2 volume);
	void GetVol(Float2 *volume);
	void SetType(ObjectType  type);
	void GetType(ObjectType*type);

private:
	Color4		m_color;			//		색깔
	Float2		m_position;
	Float2		m_volume;		//		부피
	Float2		m_velocity;
	ObjectType  m_type;								//		오브젝트 타입

};


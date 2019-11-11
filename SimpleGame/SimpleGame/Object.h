#pragma once
#include "Globals.h"

class Object
{
	
public:
	Object();
	~Object();

	void Update(CharacterStatus pos, float ElapsedTime);
	void AddForce(float x, float y, float ElapsedTime);

	//void InitPhysics();

	bool CanShootBullet();
	void ResetBulletCoolTime();

	void SetColor(Color4 color);
	void GetColor(Color4 *color);
	void SetPos(Float2 position);
	void GetPos(Float2 *position);
	void SetVel(Float2 vel);
	void GetVel(Float2*vel);
	void SetVol(Float2 volume);
	void GetVol(Float2 *volume);
	void SetType(int type);
	void GetType(int *type);

private:
	Color4		m_color;			//		색깔
	Float2		m_position;
	Float2		m_volume;		//		부피
	Float2		m_velocity;
	int m_type;								//		오브젝트 타입

	float m_RemainingCoolTime = 0.f;;
	float m_CurrentCoolTime = 0.1f;
};


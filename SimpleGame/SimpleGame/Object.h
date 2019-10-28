#pragma once
#include "Globals.h"

class Object
{
private:
	float m_posX, m_posY, m_posZ;	//location
	float m_volX, m_volY, m_volZ;	//volume
	float m_velX, m_velY, m_velZ;	//velocity
	float m_accX, m_accY, m_accZ;	//acceleration
	float m_r, m_g, m_b, m_a;		//color
	float m_mass;					//mass
	float m_fricCoef;				//friction coeff
	int m_type;

private:
	

public:
	Object();
	Object(float x, float y, float z, float size, float r, float g, float b, float a);
	~Object();

	float GetPos(float* x, float* y, float* z);
	float GetMass(float* mass);
	float GetAcc(float* x, float* y, float* z);
	float GetVel(float* x, float* y, float* z);
	float GetVol(float* x, float* y, float* z);
	float GetColor(float* r, float* g, float* b, float* a);
	void GetFricCoef(float* coef);
	void SetType(int type);
	void GetType(int* type);

	void SetPos(float x, float y, float z);
	void SetMass(float mass);
	void SetAcc(float x, float y, float z);
	void SetVel(float x, float y, float z);
	void SetVol(float x, float y, float z);
	void SetColor(float r, float g, float b, float a);
	void SetFricCoef(float coef);

public:
	void InitPhysics();
	void Update(float elapsedTimeInSec);
	void AddForce(float x, float y, float z, float elapsedTimeInSec);
};



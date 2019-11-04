#pragma once

class Object
{
	
public:
	Object();
	~Object();

	void Update(float ElapsedTime);
	void AddForce(float x, float y, float z, float ElapsedTime);

	void InitPhysics();

	bool CanShootBullet();
	void ResetBulletCoolTime();

	void SetColor(float r, float g, float b, float a);
	void GetColor(float *r, float *g, float *b, float *a);
	void SetPos(float posX, float posY, float posZ);
	void GetPos(float *posX, float *posY, float *posZ);
	void SetAcc(float accX, float accY, float accZ);
	void GetAcc(float *accX, float *accY, float *accZ);
	void SetVel(float velX, float velY, float velZ);
	void GetVel(float *velX, float *velY, float *velZ);
	void SetVol(float volX, float volY, float volZ);
	void GetVol(float *volX, float *volY, float *volZ);
	void SetMass(float mass);
	void GetMass(float *mass);
	void SetFricCoef(float coef);
	void GetFricCoef(float *coef);
	void SetType(int type);
	void GetType(int *type);

private:
	float m_r, m_g, m_b, m_a;			//		����
	float m_posX, m_posY, m_posZ;	//		��ġ
	float m_velX, m_velY, m_velZ;		//		�ӵ�
	float m_accX, m_accY, m_accZ;		//		���ӵ�
	float m_volX, m_volY, m_volZ;		//		����
	float m_mass;							//		����
	float m_fricCoef;						//		�������
	int m_type;								//		������Ʈ Ÿ��

	float m_RemainingCoolTime = 0.f;;
	float m_CurrentCoolTime = 0.1f;
};


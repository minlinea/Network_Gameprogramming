#pragma once
#include "Global.h"

enum MSG_MatchingSystem
{
	Msg_Ready = 8,
	Msg_ReadyCancel,
	Msg_ConfirmReadyCancel,
	Msg_PlayGame
};

class MatchingServer
{
private:
	std::vector<SOCKADDR_IN> m_MatchingQueue;
	unsigned char m_ClientNum;

public:
	MatchingServer();
	~MatchingServer();
	bool isMatchingQueueFull();
	void CreateGameServerThread();
	void MatchingQueueDeQueue();
	void GetClientNum(unsigned char* Data);
	void SetClientNum(const unsigned char& Data);
	void PushClient(const SOCKADDR_IN& client);
	void PopClient(const SOCKADDR_IN& client);
};

MatchingServer::MatchingServer()
{
	m_MatchingQueue.reserve(3);
	m_ClientNum = (unsigned char)0;
}

MatchingServer::~MatchingServer()
{
	m_MatchingQueue.clear();
}

bool MatchingServer::isMatchingQueueFull()
{
	if (m_ClientNum == (unsigned char)3)
		return true;
	return false;
}
void MatchingServer::CreateGameServerThread()
{

}

void MatchingServer::MatchingQueueDeQueue()
{

}

void MatchingServer::GetClientNum(unsigned char* Data)
{ 
	*Data = m_ClientNum; 
};
void MatchingServer::SetClientNum(const unsigned char& Data)
{ 
	m_ClientNum = Data;
};

void MatchingServer::PushClient(const SOCKADDR_IN& client)
{
	//m_MatchingQueue.insert(m_MatchingQueue.end(), client);
	m_ClientNum += 1;
	return;
}

void MatchingServer::PopClient(const SOCKADDR_IN& client)
{
	m_ClientNum -= 1;
	return;
}
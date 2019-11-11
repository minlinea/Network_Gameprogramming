#pragma once
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996) 
#include <vector>
#include <winsock2.h>

enum MSG_MatchingSystem
{
	Msg_Ready,
	Msg_ReadyCancel,
	Msg_ConfirmReadyCancel,
	Msg_PlayGame
};

class MatchingServer
{
private:
	std::vector<SOCKADDR> m_MatchingQueue;
	unsigned char m_ClientNum;

public:
	bool isMatchingQueueFull(int MatchingQueueCount);
	void CreateGameServerThread();
	void MatchingQueueDeQueue();
	void GetClientNum(unsigned char* Data);
	void SetClientNum(const unsigned char& Data);
	void PushClient(const SOCKADDR& client);
	void PopClient();
};

bool MatchingServer::isMatchingQueueFull(int MatchingQueueCount)
{

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

void MatchingServer::PushClient(const SOCKADDR& client)
{

}

void MatchingServer::PopClient()
{

}
#pragma once
#include "MatchingServer.h"


MatchingServer::MatchingServer()
{
	m_MatchingQueue.reserve(MAX_PLAYER);
	m_ClientNum = (unsigned char)0;
}

MatchingServer::~MatchingServer()
{
	m_MatchingQueue.clear();
}

bool MatchingServer::isMatchingQueueFull()
{
	if (m_ClientNum == (unsigned char)MAX_PLAYER)
		return true;
	return false;
}
void MatchingServer::CreateGameServerThread()
{

}

void MatchingServer::MatchingQueueDeQueue()
{
	PopClient(m_MatchingQueue[0]);
	PopClient(m_MatchingQueue[0]);
	PopClient(m_MatchingQueue[0]);
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
	m_MatchingQueue.insert(m_MatchingQueue.end(), client);
	m_ClientNum += 1;
	return;
}

void MatchingServer::PopClient(const SOCKADDR_IN& client)
{
	for (auto i = m_MatchingQueue.begin(); i != m_MatchingQueue.end(); ++i)
	{
		if (inet_ntoa(i->sin_addr) == inet_ntoa(client.sin_addr))
		{
			m_MatchingQueue.erase(i);
			break;
		}
	}
	m_ClientNum -= 1;
	return;
}

std::vector<SOCKADDR_IN> MatchingServer::GetQueue()
{
	return m_MatchingQueue;
}


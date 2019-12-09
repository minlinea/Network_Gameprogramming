#pragma once
#include "MatchingServer.h"


MatchingServer::MatchingServer()
{
	m_MatchingQueue.reserve(MAX_PLAYER);
}

MatchingServer::~MatchingServer()
{
	m_MatchingQueue.clear();
}

bool MatchingServer::isMatchingQueueFull()
{
	if (m_MatchingQueue.size() == (unsigned char)MAX_PLAYER)
		return true;
	return false;
}

void MatchingServer::GetClientNum(unsigned char* Data)
{
	*Data = m_MatchingQueue.size();
};

void MatchingServer::PushClient(const SOCKET& client)
{
	m_MatchingQueue.insert(m_MatchingQueue.end(), client);
	return;
}

void MatchingServer::PopClient(const SOCKET& client)
{
	for (auto i = m_MatchingQueue.begin(); i != m_MatchingQueue.end(); ++i)
	{
		if (*i == client)
		{
			m_MatchingQueue.erase(i);
			break;
		}
	}
	return;
}

std::vector<SOCKET> MatchingServer::GetQueue()
{
	return m_MatchingQueue;
}

void MatchingServer::ClearClient()
{
	m_MatchingQueue.clear();
}

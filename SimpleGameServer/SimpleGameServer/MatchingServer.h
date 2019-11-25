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
	std::vector<SOCKET> m_MatchingQueue;
	unsigned char m_ClientNum;

public:
	MatchingServer();
	~MatchingServer();
	bool isMatchingQueueFull();
	void CreateGameServerThread();
	void MatchingQueueDeQueue();
	void GetClientNum(unsigned char* Data);
	void SetClientNum(const unsigned char& Data);
	void PushClient(const SOCKET& client);
	void PopClient(const SOCKET& client);
	std::vector<SOCKET> GetQueue();
};
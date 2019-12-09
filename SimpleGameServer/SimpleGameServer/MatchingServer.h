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
public:
	MatchingServer();
	~MatchingServer();
	bool isMatchingQueueFull();
	void GetClientNum(unsigned char* Data);
	void PushClient(const SOCKET& client);
	void PopClient(const SOCKET& client);
	void ClearClient();
	std::vector<SOCKET> GetQueue();
};
#pragma once
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996) 
#include <winsock2.h>
#include <stdlib.h>
#include "Globals.h"


class TCPClient
{
private:
	WSADATA wsa;
	SOCKET sock;
	FixedData m_FixData;

public:
	TCPClient();

	
public:
	void err_quit(char* msg);
	void err_display(char* msg);

	int recvn(SOCKET s, char* buf, int len, int flags);

	int PlaySceneSendData(KeyInput&);

	int PlaySceneRecvData(CharacterStatus*);
};
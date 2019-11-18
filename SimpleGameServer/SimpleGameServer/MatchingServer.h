#pragma once
#include "Global.h"

enum MSG_MatchingSystem
{
	Msg_Ready = 8,
	Msg_ReadyCancel,
	Msg_ConfirmReadyCancel,
	Msg_PlayGame
};

DWORD WINAPI MatchingThread(LPVOID listen_socket);
DWORD WINAPI ClientThread(LPVOID arg);

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

MatchingServer g_Matching;
void err_quit(const char* msg);
void err_display(const char* msg);

DWORD WINAPI MatchingThread(LPVOID listen_socket)
{
	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept((SOCKET)listen_socket, (SOCKADDR*)& clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)client_sock, 0, NULL);

		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}
	}
	return 0;
}


DWORD WINAPI ClientThread(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	bool playgame = false;
	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);

	g_Matching.PushClient(clientaddr);

	while (1)
	{
		unsigned char msg;
		retval = recv(client_sock, (char*)&msg, sizeof(msg), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}

		//printf("[IP:%s	포트:%d]	msg = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), msg);

		// 현재 상태 보내기
		if (Msg_ReadyCancel == msg)
			msg = Msg_ConfirmReadyCancel;
		else if (!g_Matching.isMatchingQueueFull())
			g_Matching.GetClientNum(&msg);
		else
			msg = Msg_PlayGame;

		retval = send(client_sock, (char*)&msg, sizeof(msg), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}
		
		if (Msg_PlayGame == msg)
		{
			playgame = true;
			break;
		}
		if (Msg_ConfirmReadyCancel == msg)
		{
			break;
		}
	}

	while (playgame)
	{














	}
	//closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	g_Matching.PopClient(clientaddr);
	return 0;
}
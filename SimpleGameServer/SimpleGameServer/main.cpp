#include "Global.h"


DWORD WINAPI MatchingThread(LPVOID listen_socket);
DWORD WINAPI ClientThread(LPVOID arg);
void err_quit(const char* msg);
void err_display(const char* msg);

MatchingServer g_Matching;
CGameTimer g_Msgtimer;

int main(int argc, char* argv[])
{
	int retval;

	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	//bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)& serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	//listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");
	
	BOOL optval = TRUE;
	setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));

	// Mathcing Thread
	HANDLE hMatchingThread;
	hMatchingThread = CreateThread(NULL, 0, MatchingThread, (LPVOID)listen_sock, 0, NULL);

	while (1)
	{
		// 데이터 통신에 사용할 변수
		SOCKET client_sock;
		SOCKADDR_IN clientaddr;
		int addrlen;
		HANDLE hThread;
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept((SOCKET)listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
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
		g_Matching.PushClient(client_sock);
	}


	//closesocket()
	closesocket(listen_sock);

	//윈속 종료
	WSACleanup();
	return 0;

}

DWORD WINAPI MatchingThread(LPVOID listen_socket)
{
	g_Msgtimer.Tick(1.5f);
	while (true)
	{
		if(g_Matching.isMatchingQueueFull())
		{
			printf("풀상태\n");

			HANDLE hThread;
			
			std::vector<SOCKET> matchingqueue = g_Matching.GetQueue();

			SOCKET s[3] = { matchingqueue[0], matchingqueue[1], matchingqueue[2] };
					   			
			hThread = CreateThread(NULL, 0, GameServerThread, (LPVOID)s, 0, NULL);

			Sleep(2000);
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

	while (1)
	{
		g_Msgtimer.Tick(1.5f);
		unsigned char msg;
		retval = recv(client_sock, (char*)&msg, sizeof(msg), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}

		printf("Client Thread[IP:%s	포트:%d]	msg = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), msg);

		// 현재 상태 보내기
		if (Msg_ReadyCancel == msg)
			msg = Msg_ConfirmReadyCancel;
		else if (!g_Matching.isMatchingQueueFull())
			g_Matching.GetClientNum(&msg);
		else
			msg = Msg_PlayGame;


		//매칭스레드로부터 send해도 상관없는지 확인

		
		retval = send(client_sock, (char*)&msg, sizeof(msg), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}
		if (Msg_ConfirmReadyCancel == msg)
		{
			break;
		}

		if (msg == Msg_PlayGame)
			break;
	}

	Sleep(2000);

	printf("[TCP 서버] Client Thread 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	g_Matching.PopClient(client_sock);
	return 0;
}



// 소켓 함수 오류 출력
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
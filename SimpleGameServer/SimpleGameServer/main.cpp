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

	// Mathcing Thread
	HANDLE hMatchingThread;
	hMatchingThread = CreateThread(NULL, 0, MatchingThread, (LPVOID)listen_sock, 0, NULL);

	while (1)
	{
		;
	}

	//closesocket()
	closesocket(listen_sock);

	//윈속 종료
	WSACleanup();
	return 0;

}

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
		g_Msgtimer.Tick(1.5f);
		unsigned char msg;
		retval = recv(client_sock, (char*)&msg, sizeof(msg), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}

		printf("[IP:%s	포트:%d]	msg = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), msg);

		// 현재 상태 보내기
		if (Msg_ReadyCancel == msg)
			msg = Msg_ConfirmReadyCancel;
		else if (!g_Matching.isMatchingQueueFull())
			g_Matching.GetClientNum(&msg);
		else
			msg = Msg_PlayGame;
		printf("%d", msg);
		retval = send(client_sock, (char*)&msg, sizeof(msg), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}

		if (Msg_PlayGame == msg)
		{
			playgame = true;
			//g_Matching.PopClient(clientaddr);
			break;
		}
		if (Msg_ConfirmReadyCancel == msg)
		{
			//g_Matching.PopClient(clientaddr);
			break;
		}
	}

	while (playgame)
	{
		CommunicationThreadData* pCommData = (CommunicationThreadData*)arg;
		GameServerThreadData* pGameData = pCommData->pGameData;
		int clientNumber = pCommData->cClientNumb;
		PlayerData playerData;

		while (1)
		{
			// 데이터 받기
			retval = recv(client_sock, (char*)&pGameData->m_Players[clientNumber].KeyInput, sizeof(InputData), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 데이터 보내기에 앞서 서버가 연산 중인 데이터의 한 순간을 복사시켜서
			// 복사본을 전송하도록 한다
			// 왜 이러냐면 고정부 가변부 나눠서 데이터를 보낼건데
			// 지금 보내는 데이터는 보내는 동시에 서버에서 값이 수정되기 때문이다

			GameServerThreadData gData(*pGameData);

			// 맵변화,유저수 보내기
			retval = send(client_sock, (char*)&gData.m_fPacketH2C, sizeof(FixedData), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
			// 맵보내기
			if (gData.m_fPacketH2C.mapChanged)
			{
				retval = send(client_sock, (char*)&gData.m_MapData, sizeof(MapData), 0);
			}

			// 유저 수 만큼의 개별 유저 정보 보내기
			for (int i = 0; i < gData.m_fPacketH2C.NumOfClient; ++i)
			{
				playerData.x = gData.m_Players[i].x;
				playerData.y = gData.m_Players[i].y;
				playerData.n = gData.m_cPlayerControl[clientNumber];

				retval = send(client_sock, (char*)& playerData, sizeof(PlayerData), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("send()");
					break;
				}
			}
		}
	}
	//closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	g_Matching.PopClient(clientaddr);
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
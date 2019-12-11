#include "Global.h"


DWORD WINAPI MatchingThread(LPVOID listen_socket);
void err_quit(const char* msg);
void err_display(const char* msg);
int count = 0;				// 클라이언트 변수
CGameTimer g_Msgtimer;
HANDLE hGameServerThread;	// 게임서버스레드
SOCKET MatchingSock[3];	// 참가 대기 클라이언트


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
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("main bind()");

	//listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("main listen()");

	BOOL optval = TRUE;
	setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	//네이글 알고리즘 설정, 미 설정 시 실제로 띄엄 띄엄 움직이는 문제 발생

	// Mathcing Thread
	HANDLE hMatchingThread;
	hMatchingThread = CreateThread(NULL, 0, MatchingThread, (LPVOID)listen_sock, 0, NULL);

	while (true)
	{
		// 데이터 통신에 사용할 변수
		SOCKET client_sock;
		SOCKADDR_IN clientaddr;
		int addrlen;
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept((SOCKET)listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("main accept()");
			break;
		}
		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (MatchingSock[i] == NULL)		//빈 곳에 클라이언트 넣어주기
			{
				MatchingSock[i] = client_sock;
				break;
			}
			if (i == MAX_PLAYER - 1)
				printf("클라이언트 저장 에러");
		}
		count++;		// 참가 대기 중인 클라이언트 개수 증가
	}

	//closesocket()
	closesocket(listen_sock);

	//윈속 종료
	WSACleanup();
	return 0;
}

DWORD WINAPI MatchingThread(LPVOID listen_socket)		// 매칭 스레드, 매칭 대기 중인 클라이언트와 통신하며
{																				// 인원 충족 시, GameServerThread를 생성한다.
	g_Msgtimer.Tick(1.f);
	int retval;
	unsigned char msg;
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	while (true)
	{
		if (count == MAX_PLAYER)
		{
			printf("인원 충족\n");

			SOCKET s[3] = { MatchingSock[0], MatchingSock[1], MatchingSock[2] };

			hGameServerThread = CreateThread(NULL, 0, GameServerThread, (LPVOID)s, 0, NULL);

			count = 0;
			for (int i = 0; i < MAX_PLAYER; ++i)
			{
				MatchingSock[i] = NULL;
			}
		}
		else
		{
			g_Msgtimer.Tick(1.5f);
			for (int i = 0; i < MAX_PLAYER; ++i)
			{
				if (MatchingSock[i] != NULL)
				{
					retval = recv(MatchingSock[i], (char*)&msg, sizeof(msg), 0);
					if (retval == SOCKET_ERROR)
					{
						err_display("MatchingThread msg recv()");
						break;
					}

					getpeername(MatchingSock[i], (SOCKADDR*)&clientaddr, &addrlen);
					printf("Client Thread[IP:%s	포트:%d]	msg = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), msg);

					// 현재 상태 보내기
					if (Msg_ReadyCancel == msg)
						msg = Msg_ConfirmReadyCancel;
					else if (count != MAX_PLAYER)
						msg = count;

					retval = send(MatchingSock[i], (char*)&msg, sizeof(msg), 0);
					if (retval == SOCKET_ERROR)
					{
						err_display("MatchingThread msg send()");
						break;
					}

					if (msg == Msg_ConfirmReadyCancel)		// 클라이언트 참가 취소 확인 시 제거
					{
						MatchingSock[i] = NULL;
						count--;
					}
				}
			}
		}
	}
	return 0;
}
DWORD WINAPI GameServerThread(LPVOID arg)
{
	SOCKET* s = (SOCKET*)(arg);		//SOCKET[3]을 통해 각 클라이언트의 소켓을 전달 받는다.

	CGameTimer timer;
	GameServerThreadData gameData;

	gameData.m_Players[0].sockAddress = s[0];
	gameData.m_Players[1].sockAddress = s[1];
	gameData.m_Players[2].sockAddress = s[2];
	gameData.m_Players[0].x = -4.5f;
	gameData.m_Players[0].y = -5.0f;
	gameData.m_Players[1].x = 4.0f;
	gameData.m_Players[1].y = 1.5f;
	gameData.m_Players[2].x = -4.0f;
	gameData.m_Players[2].y = 5.5f;

	for (int i = 0; i < MAX_PLAYER; ++i)			// 컨트롤 연결
		gameData.m_cPlayerControl[i] = i;

	gameData.m_fPacketH2C.mapChanged = false;
	gameData.m_fPacketH2C.NumOfClient = MAX_PLAYER;

	int map_arr[MAP_COLUMN][MAP_ROW] = {
	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,2},
	{2,2,2,1,1,2,1,2,1,2,1,2,2,2,1,2},
	{2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
	{2,2,1,2,1,1,2,1,1,1,1,2,2,1,2,2},
	{2,2,1,1,1,2,2,1,1,2,1,1,1,1,2,2},
	{2,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2},
	{2,2,1,1,1,1,2,1,1,2,1,1,1,1,2,2},
	{2,1,1,2,1,1,2,1,1,1,1,2,1,1,1,2},
	{2,2,1,1,1,1,1,1,1,2,1,1,1,1,2,2},
	{2,1,1,2,1,1,2,1,1,2,1,2,1,1,1,2},
	{2,2,1,1,1,1,2,1,1,1,1,1,2,1,2,2},
	{2,1,1,2,1,2,1,1,1,1,1,1,1,1,1,2},
	{2,2,1,1,1,1,1,2,1,2,1,1,2,1,2,2},
	{2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2},
	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	};

	for (int i = 0; i < MAP_COLUMN; ++i)
	{
		for (int j = 0; j < MAP_ROW; ++j)
		{
			gameData.m_MapData.m_Map[i * MAP_COLUMN + j] = map_arr[i][j];
		}
	}
	gameData.MakeCommunicationThread();

	gameData.m_fPacketH2C.mapChanged = true;

	while (true)
	{
		timer.Tick(0.0f);

		Sleep(10);

		bool isEnd = true;
		for (int i = 0; i < gameData.m_fPacketH2C.NumOfClient; ++i)
		{
			if (gameData.m_Players[i].sockAddress != NULL)
				isEnd = false;
		}
		if (isEnd)
			break;
		gameData.Update(timer.GetTimeElapsed());
	}
	printf("GameServerThread 종료\n");

	return 0;
}


DWORD WINAPI ClientCommunicationThread(LPVOID arg)
{
	CommunicationThreadData* pCommData = (CommunicationThreadData*)arg;

	GameServerThreadData* pGameData = pCommData->pGameData;
	int clientNumber = pCommData->cClientNumb;

	SOCKET client_sock = pGameData->m_Players[clientNumber].sockAddress;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;

	PlayerData playerData;

	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	unsigned char msg;
	msg = Msg_PlayGame;

	retval = send(client_sock, (char*)&msg, sizeof(msg), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("ClientCommunicationThread msg recv()");
		return 0;
	}

	while (true)
	{
		//Sleep(10);

		int deadcount{ 0 };

		// 데이터 받기
		retval = recv(client_sock, (char*)&pGameData->m_Players[clientNumber].KeyInput, sizeof(InputData), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("ClientCommunicationThread KeyInput recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 데이터 보내기에 앞서 서버가 연산 중인 데이터의 한 순간을 복사하여 복사본을 전송하도록 한다.
		// 고정부 가변부 데이터를 나눠 보내는데 있어 지금 보내는 데이터는 보내는 동시 서버에서 값이 수정된다.
		// 즉, 1번 클라이언트와 3번 클라이언트가 서로 다른 맵 정보를 가지게 되는 것을 방지한다.

		GameServerThreadData gData(*pGameData);

		// 맵변화,유저수 보내기
		retval = send(client_sock, (char*)&gData.m_fPacketH2C, sizeof(FixedData), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("ClientCommunicationThread m_fPacketH2C send()");

			break;
		}

		// 맵보내기
		if (gData.m_fPacketH2C.mapChanged)
		{
			retval = send(client_sock, (char*)&gData.m_MapData, sizeof(MapData), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("ClientCommunicationThread m_MapData send()");
				break;
			}
		}

		// 유저 수 만큼의 개별 유저 정보 보내기
		for (int i = 0; i < gData.m_fPacketH2C.NumOfClient; ++i)
		{
			playerData.x = gData.m_Players[i].x;
			playerData.y = gData.m_Players[i].y;
			if (gData.m_Players[i].stat == dead)		//조종 불능 상태로 만듦
			{
				playerData.n = 3;
				++deadcount;
			}
			else
				playerData.n = gData.m_cPlayerControl[clientNumber];

			retval = send(client_sock, (char*)&playerData, sizeof(PlayerData), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("ClientCommunicationThread playerData send()");
				break;
			}
		}
	}
	for (int i = 0; i < pGameData->m_fPacketH2C.NumOfClient; ++i)
	{
		if (pGameData->m_Players[i].sockAddress == client_sock)
			pGameData->m_Players[i].sockAddress = NULL;
	}
	//closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] Communication Thread 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	return 0;
}

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
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
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
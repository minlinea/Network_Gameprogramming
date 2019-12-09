#include "Global.h"


DWORD WINAPI MatchingThread(LPVOID listen_socket);
DWORD WINAPI ClientThread(LPVOID arg);
void err_quit(const char* msg);
void err_display(const char* msg);

MatchingServer g_Matching;
CGameTimer g_Msgtimer;
HANDLE hGameServerThread;
HANDLE CommunicationThread[3];

int main(int argc, char* argv[])
{
	int retval;

	//���� �ʱ�ȭ
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
		// ������ ��ſ� ����� ����
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
		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

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

	//���� ����
	WSACleanup();
	return 0;

}

DWORD WINAPI MatchingThread(LPVOID listen_socket)
{
	bool creating = true;
	g_Msgtimer.Tick(1.5f);
	while (true)
	{
		if (g_Matching.isMatchingQueueFull() && creating)
		{
			printf("Ǯ����\n");

			hGameServerThread = CreateEvent(NULL, TRUE, TRUE, NULL);
			std::vector<SOCKET> matchingqueue = g_Matching.GetQueue();

			SOCKET s[3] = { matchingqueue[0], matchingqueue[1], matchingqueue[2] };

			hGameServerThread = CreateThread(NULL, 0, GameServerThread, (LPVOID)s, 0, NULL);

			creating = false;
		}
		if (!creating && !g_Matching.isMatchingQueueFull())
		{
			creating = true;
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
	//Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);
	unsigned char msg;
	while (1)
	{
		g_Msgtimer.Tick(1.5f);
		
		retval = recv(client_sock, (char*)&msg, sizeof(msg), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}

		printf("Client Thread[IP:%s	��Ʈ:%d]	msg = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), msg);

		// ���� ���� ������
		if (Msg_ReadyCancel == msg)
			msg = Msg_ConfirmReadyCancel;
		else if (!g_Matching.isMatchingQueueFull())
			g_Matching.GetClientNum(&msg);
		else
			msg = Msg_PlayGame;


		//��Ī������κ��� send�ص� ��������� Ȯ��

		if (msg == Msg_PlayGame)
			break;
		else
		{
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
		}
	}

		//retval = WaitForMultipleObjects(3, CommunicationThread, TRUE, INFINITE);
	if(msg == Msg_PlayGame)
		retval = WaitForSingleObject(hGameServerThread, INFINITE);
		//
	
	printf("[TCP ����] Client Thread ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	g_Matching.PopClient(client_sock);
	return 0;
}

DWORD WINAPI GameServerThread(LPVOID arg)
{
	// arg �� ���ؼ� ��Ī���� �÷��̾���� ���޹��� ���̴�
	// arg �� ���Ϲ迭 ������ ���� �� ���̴�
	// arg �� ���ؼ� gameData�� �����ϴ� ���̴�
	// �� �κ��� ���Ŀ� ��Ī ��ɰ� ������ �� �ۼ��Ѵ�

	SOCKET* s = (SOCKET*)(arg);

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

	for (int i = 0; i < MAX_PLAYER; ++i)
		gameData.m_cPlayerControl[i] = i;

	gameData.m_fPacketH2C.mapChanged = false;
	gameData.m_fPacketH2C.NumOfClient = 3;

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
	gameData.m_MapData;
	for (int i = 0; i < MAP_COLUMN; ++i)
	{
		for (int j = 0; j < MAP_ROW; ++j)
		{
			gameData.m_MapData.m_Map[i*MAP_COLUMN + j] = map_arr[i][j];
		}
	}

	gameData.MakeCommunicationThread();

	gameData.m_fPacketH2C.mapChanged = true;

	int retval = WaitForMultipleObjects(3, CommunicationThread, TRUE, INFINITE);
	
	SetEvent(hGameServerThread);
	while (1)
	{
		//printf("������Ʈ\n");

		timer.Tick(0.0f);
		// ���� ������Ʈ

		Sleep(10);

		gameData.Update(timer.GetTimeElapsed());
	}
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

	//Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);

	unsigned char msg;
	msg = Msg_PlayGame;

	retval = send(client_sock, (char*)&msg, sizeof(msg), 0);
	if (retval == SOCKET_ERROR)
	{
		return 0;
	}


	SetEvent(CommunicationThread[pCommData->cClientNumb]);
	while (1)
	{
		Sleep(10);
		// ������ �ޱ�
		retval = recv(client_sock, (char*)&pGameData->m_Players[clientNumber].KeyInput, sizeof(InputData), 0);
		if (retval == SOCKET_ERROR)
		{
			//err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ������ �����⿡ �ռ� ������ ���� ���� �������� �� ������ ������Ѽ�
		// ���纻�� �����ϵ��� �Ѵ�
		// �� �̷��ĸ� ������ ������ ������ �����͸� �����ǵ�
		// ���� ������ �����ʹ� ������ ���ÿ� �������� ���� �����Ǳ� �����̴�

		GameServerThreadData gData(*pGameData);


		// �ʺ�ȭ,������ ������
		retval = send(client_sock, (char*)&gData.m_fPacketH2C, sizeof(FixedData), 0);
		if (retval == SOCKET_ERROR)
		{
			//err_display("send()");
			break;
		}

		// �ʺ�����
		if (gData.m_fPacketH2C.mapChanged)
		{
			retval = send(client_sock, (char*)&gData.m_MapData, sizeof(MapData), 0);
		}

		// ���� �� ��ŭ�� ���� ���� ���� ������
		for (int i = 0; i < gData.m_fPacketH2C.NumOfClient; ++i)
		{
			playerData.x = gData.m_Players[i].x;
			playerData.y = gData.m_Players[i].y;
			if (gData.m_Players[i].stat == dead)
				playerData.n = 3;
			else
				playerData.n = gData.m_cPlayerControl[clientNumber];

			retval = send(client_sock, (char*)& playerData, sizeof(PlayerData), 0);
			if (retval == SOCKET_ERROR)
			{
				//err_display("send()");
				break;
			}
		}
	}

	//closesocket()
	closesocket(client_sock);
	printf("[TCP ����] Communication Thread ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

// ���� �Լ� ���� ���
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
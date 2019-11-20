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

	// Mathcing Thread
	HANDLE hMatchingThread;
	hMatchingThread = CreateThread(NULL, 0, MatchingThread, (LPVOID)listen_sock, 0, NULL);

	while (1)
	{
		;
	}

	//closesocket()
	closesocket(listen_sock);

	//���� ����
	WSACleanup();
	return 0;

}

DWORD WINAPI MatchingThread(LPVOID listen_socket)
{
	// ������ ��ſ� ����� ����
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

		printf("[IP:%s	��Ʈ:%d]	msg = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), msg);

		// ���� ���� ������
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
			// ������ �ޱ�
			retval = recv(client_sock, (char*)&pGameData->m_Players[clientNumber].KeyInput, sizeof(InputData), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
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
				err_display("send()");
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
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	g_Matching.PopClient(clientaddr);
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
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include "Global.h"
#include "MatchingServer.h"
#include "cTimer.h"
#define SERVERPORT 9000
#define BUFSIZE 512
#pragma pack(1)
struct InputData
{
	bool Up;
	bool Down;
	bool Left;
	bool Right;
	bool attack;
	bool button0;
	bool button1;
	bool button2;
	bool button3;
	bool button4;
	bool button5;
	bool button6;
	bool button7;
	bool button8;
	bool button9;
	bool button10;
};
#pragma pack()

#pragma pack(1)
struct FixedData
{
	bool mapChanged;
	char NumOfClient;
};
#pragma pack()

#pragma pack(1)
struct PlayerData
{
	float x;
	float y;
};
#pragma pack()

const int MAX_PLAYER{ 3 };

enum status
{
	dead, live
};

struct Player
{
	float x, y;
	float fSpeed{ 10.f };
	status stat;
	SOCKET clientSocket;
	InputData KeyInput;
	// ��� ���� ������ �ʿ��� ����
};

struct GAMEMAP
{

};

struct GameServerThreadData
{
	std::vector<SOCKET*> pClients;
	std::vector<Player*> pPlayers;		// �÷��̾� �������� ���� ����ü (���� ���α׷��ְ� ����)

	char		m_nCurrentPlayerAmount;
	bool		m_bMapChanged;
	CGameTimer	m_timer;				// ������Ʈ���� �����ӽð������Ҷ� ���
	GAMEMAP		m_Map;

	void MakeCommunicationThread(void) {};
};

// 1���� ���ӷ븸 �׽�Ʈ�Ϸ��� ������ ���� ����
char			NumOfClient{ 0 }; //
InputData	ClientsInput[256]; //
FixedData	fPacketH2C;
Player		Players[10];
//

DWORD WINAPI GameServerThread(LPVOID arg)
{
	CGameTimer timer;
	float fElapsedTime;
	GameServerThreadData gameData;


	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		gameData.MakeCommunicationThread();
	}

	while (1)
	{
		timer.Tick(0.0f);
		// ���� ������Ʈ

		Sleep(10);

		// �Լ�ȭ ���ϰ� �׳� �ۼ���
		fElapsedTime = timer.GetTimeElapsed();
		for (int i = 0; i < fPacketH2C.NumOfClient; ++i)
		{
			if (Players[i].KeyInput.Up)
				Players[i].y -= Players[i].fSpeed*fElapsedTime;
			if (Players[i].KeyInput.Down)
				Players[i].y += Players[i].fSpeed*fElapsedTime;
			if (Players[i].KeyInput.Left)
				Players[i].x -= Players[i].fSpeed*fElapsedTime;
			if (Players[i].KeyInput.Right)
				Players[i].x += Players[i].fSpeed*fElapsedTime;
		}

	}
}

DWORD WINAPI ClientCommunicationThread(LPVOID arg)
{
	Player* pPlayer = (Player*)arg; // ���Ӽ����������� �÷��̾� ������ ������ Ŭ���̾�Ʈ�� ����Ѵ�
	
	recv(pPlayer->clientSocket, (char*)&pPlayer->KeyInput, sizeof(InputData), 0);

	//send () �ʺ��濩�� Ŭ���̾�Ʈ����
	//send () �÷��̾ �����ϴµ� �ʿ��� ������
	return 0;
}

void update(void);

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

// ���� �Լ� ���� ���
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

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}


DWORD WINAPI ProcessClient(LPVOID arg)
{
	int clientNumber = fPacketH2C.NumOfClient;
	PlayerData playerData;

	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;

	//Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);

	while (1)
	{
		// ������ �ޱ�
		retval = recv(client_sock, (char*)&Players[clientNumber-1].KeyInput, sizeof(InputData), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// �ʺ�ȭ,������ ������
		retval = send(client_sock, (char*)& fPacketH2C, sizeof(FixedData), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}

		// �ʺ�����
		if (fPacketH2C.mapChanged)
		{
			// �� ������ ������ (����� �� false �� �׳� ����)
		}

		// ���� �� ��ŭ�� ���� ���� ���� ������
		for (int i = 0; i < fPacketH2C.NumOfClient; ++i)
		{
			playerData.x = Players[i].x;
			playerData.y = Players[i].y;
			retval = send(client_sock, (char*)& playerData, sizeof(PlayerData), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
		}
	}

	//closesocket()
	closesocket(client_sock);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

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

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	// ����ó���� ������
	HANDLE hGthread;
	hGthread = CreateThread(NULL, 0, GameServerThread, (LPVOID)NULL, 0, NULL);
	//

	fPacketH2C.NumOfClient = 0;
	fPacketH2C.mapChanged = 0;

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// ������ ����
		fPacketH2C.NumOfClient += 1;
		printf("�����ο���%d���Ǿ����\n", fPacketH2C.NumOfClient);
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}

	}

	//closesocket()
	closesocket(listen_sock);

	//���� ����
	WSACleanup();
	return 0;

}

void update(void)
{
	for (int i = 0; i < NumOfClient; ++i)
	{
		//ClientsInputs[i] ���� Ŭ���̾�Ʈ ��� ������κ��� ����
	}
}
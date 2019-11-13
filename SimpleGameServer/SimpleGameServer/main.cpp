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
	// 등등 게임 로직에 필요한 변수
};

struct GAMEMAP
{

};

struct GameServerThreadData
{
	std::vector<SOCKET*> pClients;
	std::vector<Player*> pPlayers;		// 플레이어 개개인의 상태 구조체 (소켓 프로그래밍과 무관)

	char		m_nCurrentPlayerAmount;
	bool		m_bMapChanged;
	CGameTimer	m_timer;				// 업데이트에서 프레임시간연산할때 사용
	GAMEMAP		m_Map;

	void MakeCommunicationThread(void) {};
};

// 1개의 게임룸만 테스트하려고 전역에 변수 만듬
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
		// 게임 업데이트

		Sleep(10);

		// 함수화 안하고 그냥 작성함
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
	Player* pPlayer = (Player*)arg; // 게임서버스레드의 플레이어 정보를 가지고 클라이언트와 통신한다
	
	recv(pPlayer->clientSocket, (char*)&pPlayer->KeyInput, sizeof(InputData), 0);

	//send () 맵변경여부 클라이언트길이
	//send () 플레이어가 렌더하는데 필요한 정보들
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

// 소켓 함수 오류 출력
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

// 사용자 정이 데이터 수신 함수
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

	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);

	while (1)
	{
		// 데이터 받기
		retval = recv(client_sock, (char*)&Players[clientNumber-1].KeyInput, sizeof(InputData), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 맵변화,유저수 보내기
		retval = send(client_sock, (char*)& fPacketH2C, sizeof(FixedData), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}

		// 맵보내기
		if (fPacketH2C.mapChanged)
		{
			// 맵 정보를 보낸다 (현재는 값 false 라서 그냥 놔둠)
		}

		// 유저 수 만큼의 개별 유저 정보 보내기
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
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

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

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	// 게임처리용 스레드
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
		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 스레드 생성
		fPacketH2C.NumOfClient += 1;
		printf("접속인원이%d가되었어요\n", fPacketH2C.NumOfClient);
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

	//윈속 종료
	WSACleanup();
	return 0;

}

void update(void)
{
	for (int i = 0; i < NumOfClient; ++i)
	{
		//ClientsInputs[i] 값을 클라이언트 통신 쓰레드로부터 대입
	}
}
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    524288

#pragma pack(1)
typedef struct InputData
{
	bool Up;
	bool Down;
	bool Left;
	bool Right;
	bool Interact1;
	bool Interact2;
	bool Interact3;
	bool Interact4;
}
InputData;
#pragma pack()

// ���� �Լ� ���� ��� �� ����
void err_quit(const char *msg)
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

// ���� �Լ� ���� ���
void err_display(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0)
	{
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

int main(int argc, char *argv[])
{
	char SVIP[16];
	ZeroMemory(SVIP, 16);

	printf("type server IP:");
	scanf("%s", SVIP);
	getchar();

	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SVIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	InputData myInput;
	InputData entireUserInput[256];
	char NumofUser;

	// ������Ʈ����
	while (1)
	{
		myInput.Up = false;
		myInput.Down = false;
		myInput.Left = false;
		myInput.Right = false;
		myInput.Interact1 = false;
		myInput.Interact2 = false;
		myInput.Interact3 = false;
		myInput.Interact4 = false;

		//
		// Ű ����
		if (GetAsyncKeyState(VK_UP) & 0x8000)
			myInput.Up = 1;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			myInput.Down = 1;
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			myInput.Left = 1;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			myInput.Right = 1;
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			myInput.Interact1 = 1;
		//

		send(sock, (char*)& myInput, sizeof(myInput), 0);

		recv(sock, (char*)& NumofUser, sizeof(NumofUser), 0);

		for (int i = 0; i < NumofUser; ++i)
			recv(sock, (char*)& entireUserInput[i], sizeof(InputData), 0);

		// ��º�
		system("cls");
		for (int i = 0; i < NumofUser; ++i)
			printf("\n###%d�� ����###\nUp\t%d\nDown\t%d\nLeft\t%d\nRight\t%d\n", i, entireUserInput[i].Up, entireUserInput[i].Down, entireUserInput[i].Left, entireUserInput[i].Right);
		// ��º�
	}
	// ������Ʈ����

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();

	getchar();

	return 0;
}
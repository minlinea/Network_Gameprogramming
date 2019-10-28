#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996) 
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#define SERVERPORT 9000
#define BUFSIZE 512

#pragma pack(1)   //구조체 멤버 맞춤을 1바이트 경계로 변경
struct FileData {
	char name[255];
	unsigned int byte;
};
#pragma pack()	// 구조체 멤버 맞춤을 기본값으로 환원
#pragma pack(1)
struct InputData
{
	bool Up;
	bool Down;
	bool Left;
	bool Right;
	bool Interact1;
	bool Interact2;
	bool Interact3;
	bool Interact4;
};
#pragma pack()
//	global variable
char			NumOfClients	{ 0 };
InputData	ClientsInputs[4];
//

void update(void);

void err_quit(char* msg)
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
void err_display(char* msg)
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
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	InputData data;

	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);

	while (1) {
		// 데이터 받기
		retval = recv(client_sock,(char*)&data, sizeof(data), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;
		/*data.Down = 1;
		data.Up = 1;
		data.Right = 1;
		data.Left = 1;
		data.Interact1 = 1;*/

		//받은 데이터 출력
		printf("%d %d %d %d %d %d %d %d \n", data.Up, data.Down,  data.Left, data.Right,data.Interact1, data.Interact2, data.Interact3, data.Interact4);
		
		//데이터 보내기
		retval = send(client_sock, (char*)& data, sizeof(data), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
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

	for (int i = 0; i < 2;++i) {
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
		NumOfClients += 1;
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
		
	}

	while (1)
		update();

	//closesocket()
	closesocket(listen_sock);

	//윈속 종료
	WSACleanup();
	return 0;

}

void update(void)
{
	for (int i = 0; i < NumOfClients; ++i)
	{
		//ClientsInputs[i] 값을 클라이언트 통신 쓰레드로부터 대입
	}
}
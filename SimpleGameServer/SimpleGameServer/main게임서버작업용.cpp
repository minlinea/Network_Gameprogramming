#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include "Global.h"
#include "MatchingServer.h"
#define SERVERPORT 9000
#define BUFSIZE 512


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
	// 게임 방을 만드는 단계다
	//
	// 현재 매칭 단계와 분리하여 작성중이다
	// 매칭 단계 없이 작동하도록 한다
	// 단 한 개의 게임 방을 만들고 연결되는 클라이언트를 여기에 집어넣는다
	HANDLE hGthread;
	hGthread = CreateThread(NULL, 0, GameServerThread, (LPVOID)NULL, 0, NULL);
	//


	//while (1) {
	//	// accept()
	//	addrlen = sizeof(clientaddr);
	//	client_sock = accept(listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
	//	if (client_sock == INVALID_SOCKET) {
	//		err_display("accept()");
	//		break;
	//	}
	//	// 접속한 클라이언트 정보 출력
	//	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	//	// 스레드 생성
	//	fPacketH2C.NumOfClient += 1;
	//	printf("접속인원이%d가되었어요\n", fPacketH2C.NumOfClient);
	//	hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
	//	if (hThread == NULL)
	//	{
	//		closesocket(client_sock);
	//	}
	//	else
	//	{
	//		CloseHandle(hThread);
	//	}

	//}


	//closesocket()
	closesocket(listen_sock);

	//윈속 종료
	WSACleanup();
	return 0;

}
#include "stdafx.h"
#include "TCPClientClass.h"
#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 524288
#include<iostream>
//template<typename Data_Type>
void TCPClientClass::err_quit(char* msg)
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
//template<typename Data_Type>
void TCPClientClass::err_display(char* msg)
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
//template<typename Data_Type>
int TCPClientClass::recvn(SOCKET s, char* buf, int len, int flags)
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

//template<typename Data_Type>
int TCPClientClass::SendData(InputData* data)
{
	int retval;
	//std::cout << data->Up << data->Down << data->Left << data->Right << data->Interact1 << data->Interact2 << data->Interact3 << data->Interact4 << std::endl;
	retval = send(sock, (char*)& data, sizeof(data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return 0;
	}
	return 1;
}

//template<typename Data_Type>
int TCPClientClass::RecvData(InputData* data)
{
	int retval;
	retval = recvn(sock, (char*) &data, sizeof(data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return 0;
	}
	std::cout << data->Up << data->Down << data->Left << data->Right << data->Interact1 << data->Interact2 << data->Interact3 << data->Interact4 << std::endl;

	return 1;
}

//template<typename Data_Type>
TCPClientClass::TCPClientClass()
{
	int retval;

	//���� �ʱ�ȭ
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit("WSAStartup()");

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)& serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
	else { printf("���� ���� ����\n"); }
}


#pragma once
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996) 
#include <winsock2.h>
#include <stdlib.h>

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
//template<typename Data_Type>
class TCPClientClass
{
private:
	WSADATA wsa;
	SOCKET sock;

public:
	TCPClientClass();

public:
	void err_quit(char* msg);
	void err_display(char* msg);

	int recvn(SOCKET s, char* buf, int len, int flags);
	
	int SendData(InputData*);
	int RecvData(InputData*);
};


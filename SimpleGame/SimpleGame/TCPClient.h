#pragma once
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996) 
#include <winsock2.h>
#include <stdlib.h>
#include "Globals.h"


class TCPClient
{
private:
	WSADATA wsa;
	SOCKET sock;
	FixedData m_FixData;					//PlayScene�� �����͸� ���� ���� �������� ���� 
	MapData m_Map;							//�������� ������ true�� �Ѿ�´ٸ� �޴� ���� ������

public:
	TCPClient();							//���� �� �����ʱ�ȭ�� ���� ����, connect ����
	~TCPClient();							//�Ҹ� �� closesocket�� �������� ����

	//���� ���� ��� �Լ�
	void err_quit(char* msg);
	void err_display(char* msg);
	
	int recvn(SOCKET s, char* buf, int len, int flags);

public:
	int TitleSceneSendData(unsigned char msg);		//Msg_Ready�� Msg_ReadyCancel�� MatchingThread�� ����

	int TitleSceneRecvData(unsigned char &msg);		
	/*MatchingThrea�κ��� �޼����� �޴´�. 
	ClientNum: ��Ī ������� Ŭ���̾�Ʈ ���� �޴´�.
	Msg_ConfirmReadyCancel: Msg_ReadyCancel ���� ��, ���� �ʿ��� ������ Ȯ�ε����� Ȯ���ϰ� ���� ����
	Msg_PlayGame: PlayScene���� ��ȯ
	*/
	int PlaySceneSendData(KeyInput&);

	int PlaySceneRecvData(CharacterStatus* data);

	MapData GetMap();

};
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
	FixedData m_FixData;					//PlayScene의 데이터를 받을 때의 고정길이 변수 
	MapData m_Map;							//고정길이 변수에 true로 넘어온다면 받는 맵의 데이터

public:
	TCPClient();							//생성 시 윈속초기화와 소켓 생성, connect 수행
	~TCPClient();							//소멸 시 closesocket과 윈속종료 수행

	//에러 내용 출력 함수
	void err_quit(char* msg);
	void err_display(char* msg);
	
	int recvn(SOCKET s, char* buf, int len, int flags);

public:
	int TitleSceneSendData(unsigned char msg);		//Msg_Ready나 Msg_ReadyCancel을 MatchingThread에 전송

	int TitleSceneRecvData(unsigned char &msg);		
	/*MatchingThrea로부터 메세지를 받는다. 
	ClientNum: 매칭 대기중인 클라이언트 수를 받는다.
	Msg_ConfirmReadyCancel: Msg_ReadyCancel 전송 후, 서버 쪽에서 수신이 확인됐음을 확인하고 연결 종료
	Msg_PlayGame: PlayScene으로 전환
	*/
	int PlaySceneSendData(KeyInput&);

	int PlaySceneRecvData(CharacterStatus* data);

	MapData GetMap();

};
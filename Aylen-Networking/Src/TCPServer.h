#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "CommunicationBasics.h"

#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>

#include <thread>
#include <atomic>
#include <string>

class TCPServer : public CommunicationBasics{
public:
	TCPServer();
	virtual ~TCPServer();
	int Init();
	void Stop();

	TCP_SERVER_STATE getState(){ return state; };
	void SetListeningPort(char* newPort);
	void SetListeningPort(int newPort);

	void SetDebugState(bool debugState);

private:
	void Start();
	int Shutdown();
	int ProcessMessage(int recvLen);
	void BuildRetPingAnswer();
	void BuildRetErrorAnswer();
	void BuildRetSimpleAnswer();
	void BuildRetSimpleAnswer(int res);
	void BuildRetSimpleAnswer(int res1, char* res2);
	void BuildRetSimpleAnswer(int res1, double res2[]);
	bool StrToBool(char* value);

private:
	SOCKET ClientSocket;
	SOCKET ListenSocket;
	TCP_SERVER_STATE state;
	std::thread* ServerThread;
	
	int listeningPort;

	char recvbuf[DEFAULT_TCP_RECVBUFLEN];
	int recvbuflen = DEFAULT_TCP_RECVBUFLEN - 1;

	char sendbuf[DEFAULT_TCP_SENDBUFLEN];
	int sendbuflen = DEFAULT_TCP_SENDBUFLEN - 1;

	std::atomic<bool> debug;
};

#endif
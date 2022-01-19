#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include "CommunicationBasics.h"

#include <thread>
#include <atomic>

class UDPSender : public CommunicationBasics{
public:
	UDPSender();
	virtual ~UDPSender();
	int Init();
	void Stop();

	UDP_SENDER_STATE getState(){ return state; };
	void SetReceiverPort(char* newPort);
	void SetReceiverPort(int newPort);
	void SetReceiverIP(char* newIP);

	void SetSendingState(bool sendState);
	void SetDebugState(bool debugState);

private:
	void Start();
	int Shutdown();
	int PrepareMessage();

private:
	SOCKET SendingSocket;
	UDP_SENDER_STATE state;
	std::thread* SenderThread;

	int receiverPort;
	char const* receiverIP;
	SOCKADDR_IN ReceiverAddr;

	char sendbuf[DEFAULT_UDP_SENDBUFLEN];
	int sendbuflen = DEFAULT_UDP_SENDBUFLEN - 1;

	std::atomic<bool> sending;
	std::atomic<bool> debug;
	std::atomic<bool> running;
};

#endif
#undef UNICODE
#ifndef COMMUNICATION_BASICS_H
#define COMMUNICATION_BASICS_H

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <stdio.h>

#include "AppVirtsa.h"

#define DEFAULT_TCP_RECVBUFLEN 1024
#define DEFAULT_TCP_SENDBUFLEN 1024

#define DEFAULT_UDP_SENDBUFLEN 1024

#define DEFAULT_TCP_PORT 29910
#define DEFAULT_UDP_PORT 29900

#define DEFAULT_UDP_SENDER_IP "127.0.0.1"
#define DEFAULT_TCP_SERVER_IP "127.0.0.1"

enum UDP_SENDER_STATE{
	UDP_NOT_INITIALIZED,
	UDP_INITIALIZED,
	UDP_INITIALIZATION_ERROR,
	UDP_SENDING_DATAGRAMS,
	UDP_COMMUNICATION_ERROR,
	UDP_COMMUNICATION_ENDED
};

enum TCP_SERVER_STATE{
	TCP_NOT_INITIALIZED,
	TCP_INITIALIZED,
	TCP_INITIALIZATION_ERROR,
	TCP_LISTENING,
	TCP_CONNECTING_ERROR,
	TCP_CLIENT_CONNECTED,
	TCP_CLIENT_DISCONNECTED,
	TCP_COMMUNICATION_ERROR,
	TCP_CONNECTION_CLOSED
};

class CommunicationBasics{

public:
	CommunicationBasics();
	virtual ~CommunicationBasics();
	char* GetLastErrorMessage();

	void SetAppManager(AppVirtsa *app);

protected:
	char lastErrorMsg[255];

	AppVirtsa *AppManager;
};

#endif
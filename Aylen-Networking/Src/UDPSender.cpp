#include "UDPSender.h"
#include <ws2tcpip.h>

UDPSender::UDPSender(){
	state = UDP_NOT_INITIALIZED;
	SenderThread = NULL;
	SendingSocket = INVALID_SOCKET;
	receiverPort = DEFAULT_UDP_PORT;
	receiverIP = DEFAULT_UDP_SENDER_IP;
	sending = true;
	running = true;
}


UDPSender::~UDPSender(){
	Stop();
}


int UDPSender::Init(){
	printf(":: UDP_SENDER :: init\n\tReceiver = (ip = %s ; port = %d)\n", receiverIP, receiverPort);

	state = UDP_NOT_INITIALIZED;

	WSADATA wsaData;
	int iResult;

	// set initial socket state
	SendingSocket = INVALID_SOCKET;

	// Initialize Winsock version 2.2
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0){
		sprintf_s(lastErrorMsg, "WSAStartup failed with error: %d\n", iResult);
		WSACleanup();

		printf(":: UDP_SENDER ERROR ::\n");
		printf("%s", GetLastErrorMessage());

		state = UDP_INITIALIZATION_ERROR;
		return 1;
	}

	// Set up a SOCKADDR_IN structure that will identify who we
	// will send datagrams to.
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(receiverPort);
	void* buffer{};
	switch (inet_pton(AF_INET, receiverIP, buffer)) {
	case 0:
		// The format of the given address is incorrect (IPV6 when IPV4 is expected
		// for instance.
		std::cout << "Wrong format for the given IP address";
		break;
	case -1:
		// The type of address specified by the first parameter of inet_pton
		// is not supported.
		std::cout << "IP address type not supported";
		break;
	default:
		// Everything went fine!
		ReceiverAddr.sin_addr.s_addr = *static_cast<ULONG*>(buffer);
	}

	// Create a new socket to send datagrams on.
	SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendingSocket == INVALID_SOCKET){
		sprintf_s(lastErrorMsg, "socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();

		printf(":: UDP_SENDER ERROR ::\n");
		printf("%s", GetLastErrorMessage());

		state = UDP_INITIALIZATION_ERROR;
		return 1;
	}

	state = UDP_INITIALIZED;

	running = true;
	// start thread to wait for client and do communication
	SenderThread = new std::thread(&UDPSender::Start, this);

	return 0;
}


void UDPSender::Stop(){
	printf(":: UDP_SENDER :: Stop demanded! :: possible error message\n");
	switch (state){
		case UDP_INITIALIZED:
		case UDP_SENDING_DATAGRAMS:
			closesocket(SendingSocket);
			WSACleanup();
			break;
		default:
			break;
	}
	state = UDP_NOT_INITIALIZED;

	// clean thread
	if (SenderThread != NULL){
		running = false;
		if (SenderThread->joinable()){
			SenderThread->join();
		}
		delete SenderThread;
		SenderThread = NULL;
	}
}


void UDPSender::SetReceiverPort(char * newPort){ receiverPort = atoi(newPort); }


void UDPSender::SetReceiverPort(int newPort){ receiverPort = newPort; }


void UDPSender::SetReceiverIP(char * newIP){ receiverIP = newIP; }


void UDPSender::SetSendingState(bool sendState){
	printf(":: UDP_SENDER :: update state :: send? %s\n", (sendState ? "Yes" : "No"));
	sending = sendState;
}

void UDPSender::SetDebugState(bool debugState){ debug = debugState; }


void UDPSender::Start(){
	printf(":: UDP_SENDER :: sending datagrams state :: send? %s\n", (sending ? "Yes" : "No"));
	state = UDP_SENDING_DATAGRAMS;

	int count = 10;
	while (running){
		while (!sending && running){
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		if (!running) continue;

		// Send a datagram to the receiver
		int haveMessage = PrepareMessage();
		if (haveMessage == -1) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}
		//sprintf_s(sendbuf, "Dummy testing data . . .");
		//printf("\tData to be sent: \"%s\"\n", sendbuf);

		int iSendResult = sendto(SendingSocket, sendbuf, sendbuflen, 0,
			(SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr));
		if (iSendResult == SOCKET_ERROR){
			sprintf_s(lastErrorMsg, "send failed with error: %d\n", WSAGetLastError());
			closesocket(SendingSocket);
			WSACleanup();

			printf(":: UDP_SERVER ERROR ::\n");
			printf("%s", GetLastErrorMessage());

			state = UDP_COMMUNICATION_ERROR;
			return;
		}
		//printf("\tBytes sent: %d\n", iSendResult);
	}

	// close the connection
	Shutdown();
}


int UDPSender::Shutdown(){
	printf(":: UDP_SENDER :: Shutdown\n");

	// cleanup
	closesocket(SendingSocket);
	WSACleanup();

	printf(":: UDP_SENDER :: Connection closed\n");
	state = UDP_COMMUNICATION_ENDED;
	return 0;
}


int UDPSender::PrepareMessage(){
	double outInformation[255];
	int res1 = AppManager->GetInteractObjectsData(outInformation);
	if (res1 == -1)
		return res1;
	memset(sendbuf, '\0', DEFAULT_TCP_SENDBUFLEN);
	std::stringstream ss;
	for (int i = 0; i < res1; ++i){
		ss << " ";
		ss << outInformation[i];
	}
	sprintf_s(sendbuf, "OK %d%s\0", res1, ss.str().c_str());
	sendbuflen = strlen(sendbuf);
	return res1;
}

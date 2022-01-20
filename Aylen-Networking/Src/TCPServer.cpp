#include "TCPServer.h"
#include <vector>
#include <sstream>

TCPServer::TCPServer(){
	state = TCP_NOT_INITIALIZED;
	ServerThread = NULL;
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	listeningPort = DEFAULT_TCP_PORT;
	debug = true;
}


TCPServer::~TCPServer(){
	Stop();
}


int TCPServer::Init(){
	printf(":: TCP_SERVER :: init\n\tListening port = %d\n", listeningPort);

	state = TCP_NOT_INITIALIZED;

	WSADATA wsaData;
	int iResult;

	// set initial socket state
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	// Initialize Winsock version 2.2
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0){
		sprintf_s(lastErrorMsg, "WSAStartup failed with error: %d\n", iResult);
		WSACleanup();

		printf(":: TCP_SERVER ERROR ::\n");
		printf("%s", GetLastErrorMessage());

		state = TCP_INITIALIZATION_ERROR;
		return 1;
	}

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, std::to_string(listeningPort).c_str(), &hints, &result);
	if (iResult != 0){
		sprintf_s(lastErrorMsg, "getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();

		printf(":: TCP_SERVER ERROR ::\n");
		printf("%s", GetLastErrorMessage());

		state = TCP_INITIALIZATION_ERROR;
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET){
		sprintf_s(lastErrorMsg, "socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();

		printf(":: TCP_SERVER ERROR ::\n");
		printf("%s", GetLastErrorMessage());

		state = TCP_INITIALIZATION_ERROR;
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR){
		sprintf_s(lastErrorMsg, "bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();

		printf(":: TCP_SERVER ERROR ::\n");
		printf("%s", GetLastErrorMessage());

		state = TCP_INITIALIZATION_ERROR;
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR){
		sprintf_s(lastErrorMsg, "listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();

		printf(":: TCP_SERVER ERROR ::\n");
		printf("%s", GetLastErrorMessage());

		state = TCP_INITIALIZATION_ERROR;
		return 1;
	}

	state = TCP_INITIALIZED;

	// start thread to wait for client and do communication
	ServerThread = new std::thread(&TCPServer::Start, this);

	return 0;
}


void TCPServer::Stop(){
	printf(":: TCP_SERVER :: Stop demanded! :: possible error message\n");
	switch (state){
		case TCP_INITIALIZED:
		case TCP_LISTENING:
			closesocket(ListenSocket);
			WSACleanup();
			break;
		case TCP_CLIENT_CONNECTED:
			closesocket(ClientSocket);
			WSACleanup();
			break;
		default:
			break;
	}
	state = TCP_NOT_INITIALIZED;

	// clean thread
	if (ServerThread != NULL){
		if (ServerThread->joinable()){
			ServerThread->join();
		}
		delete ServerThread;
		ServerThread = NULL;
	}
}


void TCPServer::SetListeningPort(char * newPort){ listeningPort = atoi(newPort); }


void TCPServer::SetListeningPort(int newPort){ listeningPort = newPort; }

void TCPServer::SetDebugState(bool debugState){ debug = debugState; }


void TCPServer::Start(){
	printf(":: TCP_SERVER :: waiting for client\n");
	state = TCP_LISTENING;

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET){
		sprintf_s(lastErrorMsg, "accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();

		printf(":: TCP_SERVER ERROR ::\n");
		printf("%s", GetLastErrorMessage());

		state = TCP_CONNECTING_ERROR;
		return;
	}

	printf(":: TCP_SERVER :: client connected\n");

	// No longer need server socket
	closesocket(ListenSocket);

	state = TCP_CLIENT_CONNECTED;

	// Receive until the peer shuts down the connection
	int iResult = -1;
	do{
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0){
			// Process client message
			ProcessMessage(iResult);

			// Send answer
			int iSendResult = send(ClientSocket, sendbuf, sendbuflen, 0);
			if (iSendResult == SOCKET_ERROR){
				sprintf_s(lastErrorMsg, "send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();

				printf(":: TCP_SERVER ERROR ::\n");
				printf("%s", GetLastErrorMessage());

				state = TCP_COMMUNICATION_ERROR;
				return;
			}
			if (debug) printf("\tAnswer: %s\n\tBytes (to send ; sent): (%d ; %d)\n", sendbuf, sendbuflen, iSendResult);

		}
		else if (iResult == 0){
			printf(":: TCP_SERVER :: client disconnected\n");

			state = TCP_CLIENT_DISCONNECTED;
		}
		else{
			sprintf_s(lastErrorMsg, "recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();

			printf(":: TCP_SERVER ERROR ::\n");
			printf("%s", GetLastErrorMessage());

			state = TCP_COMMUNICATION_ERROR;
			return;
		}

	} while (iResult > 0);

	// close the connection
	Shutdown();
}


int TCPServer::Shutdown(){
	printf(":: TCP_SERVER :: Shutdown\n");

	// shutdown the connection
	int iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR){
		sprintf_s(lastErrorMsg, "shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();

		printf(":: TCP_SERVER ERROR ::\n");
		printf("%s", GetLastErrorMessage());

		state = TCP_CONNECTION_CLOSED;
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	printf(":: TCP_SERVER :: Connection closed\n");
	state = TCP_CONNECTION_CLOSED;
	return 0;
}


int TCPServer::ProcessMessage(int recvLen){
	// set end of message
	recvbuf[recvLen] = '\0';
	if (debug) printf("\tMessage received: %s\n\tBytes received: %d\n", recvbuf, recvLen);

	// separate buffer by spaces
	std::vector<char*> v;
	char** temp;
	char* chars_array = strtok_s(recvbuf, " ", temp);
	while (chars_array){
		v.push_back(chars_array);
		chars_array = strtok_s(NULL, " ", temp);
	}

	/*if (debug){
		printf("vector size = %d\n", v.size());
		printf("Content:\n----\n");
		for (size_t n = 0; n < v.size(); ++n){
			printf(" %s\n", v[n]);
		}
		printf("---\n");
	}*/

	if (v.size() < 2){
		printf(":: TCP_SERVER :: message shorter than minimum\n");
		BuildRetErrorAnswer();
		return 1;
	}

	int funcCmd = atoi(v[0]);
	int numParams = atoi(v[1]);

	if (debug){
		printf("Function = %d\n", funcCmd);
		printf("#Params = %d\n", numParams);
	}

	if (funcCmd >= 0 && numParams >= 0 && numParams == v.size() - 2){
		int paramsQty = v.size() - 2;
		int res = 0;
		double outInformation[255];

		// Apply funcCmd with numParams
		switch (funcCmd){
			case 0:
				BuildRetPingAnswer();
				break;
			case 1:		// InitializeVirtsa
				if (paramsQty != 5){
					printf(":: TCP_SERVER :: InitializeVirtsa :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: InitializeVirtsa\n");
				/*bool debugConsole = StrToBool(v[2]);
				bool showGraphics = StrToBool(v[3]);
				bool debugGraphics = StrToBool(v[4]);
				int arduinoSerialPort = atoi(v[5]);
				int arduinoBaudRate = atoi(v[6]);*/

				//res = AppManager->InitApp(StrToBool(v[2]), StrToBool(v[3]), StrToBool(v[4]), atoi(v[5]), atoi(v[6]));
				res = AppManager->InitAppSceneAndStart(StrToBool(v[2]), StrToBool(v[3]), StrToBool(v[4]), atoi(v[5]), atoi(v[6]));
				res = 2;// AppManager->getToolsNumber();
				BuildRetSimpleAnswer(res);
				break;
			case 2:		// SetVirtualScene
				if (paramsQty != 0){
					printf(":: TCP_SERVER :: SetVirtualScene :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: SetVirtualScene\n");
				res = AppManager->InitScene();
				BuildRetSimpleAnswer(res);
				break;
			case 3:		// StartVirtsa
				if (paramsQty != 0){
					printf(":: TCP_SERVER :: StartVirtsa :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: StartVirtsa\n");
				res = AppManager->StartMyApp();
				BuildRetSimpleAnswer(res);
				break;
			case 4:		// finishVirtsa
				if (paramsQty != 0){
					printf(":: TCP_SERVER :: finishVirtsa :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: finishVirtsa\n");
				res = AppManager->ShutDownApp();
				BuildRetSimpleAnswer(res);
				break;
			case 5:		// UpdateGameScenePosition
				if (paramsQty != 3){
					printf(":: TCP_SERVER :: UpdateGameScenePosition :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: UpdateGameScenePosition\n");
				/*double x = strtod(v[2], NULL);
				double y = strtod(v[3], NULL);
				double z = strtod(v[4], NULL);*/
				res = AppManager->SetGameScenePosition(strtod(v[2], NULL), strtod(v[3], NULL), strtod(v[4], NULL));
				BuildRetSimpleAnswer(res);
				break;
			case 6:		// SetHapticForces
				if (paramsQty != 1){
					printf(":: TCP_SERVER :: SetHapticForces :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: SetHapticForces\n");
				//bool enable = StrToBool(v[2]);
				res = AppManager->SetForces(StrToBool(v[2]));
				BuildRetSimpleAnswer(res);
				break;
			case 7:		// resetObjectsPositions
				if (paramsQty != 0){
					printf(":: TCP_SERVER :: resetObjectsPositions :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: resetObjectsPositions\n");
				res = AppManager->ResetGamePositions();
				BuildRetSimpleAnswer(res);
				break;
			case 8:		// getNumberOfTools
				if (paramsQty != 0){
					printf(":: TCP_SERVER :: getNumberOfTools :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: getNumberOfTools\n");
				res = 2;//AppManager->getToolsNumber();
				BuildRetSimpleAnswer(res);
				break;
			case 9:		// GetInteractObjects
				if (paramsQty != 0){
					printf(":: TCP_SERVER :: GetInteractObjects :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: GetInteractObjects -- TO DO\n");
				res = AppManager->GetInteractObjectsData(outInformation);
				BuildRetSimpleAnswer(res, outInformation);
				break;
			case 10:	// GetHapticInformation
				if (paramsQty != 0){
					printf(":: TCP_SERVER :: GetHapticInformation :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: GetHapticInformation -- TO DO\n");
				res = AppManager->GetHapticsData(outInformation);
				BuildRetSimpleAnswer(res, outInformation);
				break;
			case 11:	// GetRigidObjects
				if (paramsQty != 0) {
					printf(":: TCP_SERVER :: GetRigidObjects :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: GetRigidObjects -- TO DO\n");
				res = AppManager->GetRigidObjectsData(outInformation);
				BuildRetSimpleAnswer(res, outInformation);
				break;
			case 12:	// UpdateDynamics
				if (paramsQty != 2) {
					printf(":: TCP_SERVER :: UpdateDynamics :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: UpdateDynamics\n");
				res = AppManager->UpdateCubeDynamics(atoi(v[2]), StrToBool(v[3]));
				BuildRetSimpleAnswer(res, outInformation);
				break;
			case 13:		// resetObjectPosition
				if (paramsQty != 1) {
					printf(":: TCP_SERVER :: resetObjectPosition :: INVALID_NUMBER_OF_PARAMETERS\n");
					BuildRetErrorAnswer();
					return 1;
				}
				printf(":: TCP_SERVER :: resetObjectPosition\n");
				res = AppManager->ResetCubePosition(atoi(v[2]));
				BuildRetSimpleAnswer(res);
				break;
			default:
				printf(":: TCP_SERVER :: wrong message :: FUNCTION_NOT_IMPLEMENTED\n");
				BuildRetErrorAnswer();
				return 1;
				break;
		}
		return 0;
	}

	if (funcCmd <= 0){
		printf(":: TCP_SERVER :: wrong message :: INVALID_FUNCTION\n");
	}
	else
		if (numParams < 0){
			printf(":: TCP_SERVER :: wrong message :: INVALID_PARAMETERS_SIZE\n");
		}
		else
			if (numParams > v.size() - 2){
				printf(":: TCP_SERVER :: wrong message :: INVALID_NUMBER_OF_PARAMETERS\n");
			}
			else
				if (numParams > v.size() - 2){
					printf(":: TCP_SERVER :: message shorter than expected or wrong message :: INVALID_NUMBER_OF_PARAMETERS\n");
				}
				else
					printf(":: TCP_SERVER :: message error :: UNKNOWN\n");

	BuildRetErrorAnswer();
	return 1;
}


void TCPServer::BuildRetPingAnswer() {
	memset(sendbuf, '\0', DEFAULT_TCP_SENDBUFLEN);
	sprintf_s(sendbuf, "PING ACK");
	sendbuflen = 8;
}


void TCPServer::BuildRetErrorAnswer() {
	memset(sendbuf, '\0', DEFAULT_TCP_SENDBUFLEN);
	sprintf_s(sendbuf, "ERROR 0");
	sendbuflen = 7;
}


void TCPServer::BuildRetSimpleAnswer(){
	memset(sendbuf, '\0', DEFAULT_TCP_SENDBUFLEN);
	sprintf_s(sendbuf, "OK 0");
	sendbuflen = 4;
}


void TCPServer::BuildRetSimpleAnswer(int res){
	memset(sendbuf, '\0', DEFAULT_TCP_SENDBUFLEN);
	sprintf_s(sendbuf, "OK %d", res);
	sendbuflen = strlen(sendbuf);
}


void TCPServer::BuildRetSimpleAnswer(int res1, char* res2){
	memset(sendbuf, '\0', DEFAULT_TCP_SENDBUFLEN);
	sprintf_s(sendbuf, "OK %d %s", res1, res2);
	sendbuflen = strlen(sendbuf);
}

void TCPServer::BuildRetSimpleAnswer(int res1, double res2[]){
	memset(sendbuf, '\0', DEFAULT_TCP_SENDBUFLEN);
	std::stringstream ss;
	for (int i = 0; i < res1; ++i){
		ss << " ";
		ss << res2[i];
	}
	sprintf_s(sendbuf, "OK %d%s", res1, ss.str().c_str());
	sendbuflen = strlen(sendbuf);
}


bool TCPServer::StrToBool(char * value){
	bool ret = false;
	std::stringstream ss(value);
	ss >> std::boolalpha >> ret;
	return ret;
}

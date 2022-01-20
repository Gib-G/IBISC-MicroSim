#include "TCPServer.h"
#include "UDPSender.h"
#include "CommunicationBasics.h"
#include <iostream>

// Need to link with Ws2_32.lib for sockets
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void help(){
	cout << endl;
	cout << endl;
	cout << "-----------------------------------------" << endl;
	cout << " >>>>> Help: " << endl;
	cout << "-----------------------------------------" << endl;
	cout << " [0] > EXIT Application" << endl;
	cout << " [1] > Show Help" << endl;
	cout << " [2] > TCP Server Shutdown" << endl;
	cout << " [3] > TCP Server Manual Reconnection" << endl;
	cout << " [4] > UDP Sender Shutdown" << endl;
	cout << " [5] > UDP Sender Connection" << endl;
	cout << " [6] > Start/Resume UDP sending" << endl;
	cout << " [7] > Pause UDP sending" << endl;
	cout << " [8] > Show application state [TO DO]" << endl;
	cout << " [9] > " << endl;
	cout << "-----------------------------------------" << endl;
	cout << endl;
	cout << endl;
}


void welcome(){
	cout << endl;
	cout << endl;
	cout << "\t|---------------------------------------|" << endl;
	cout << "\t|  ViRTSA .::. PICK TRANSFER AND PLACE  |" << endl;
	cout << "\t|                     Aylen Ricca 2019  |" << endl;
	cout << "\t|                                -v1.0  |" << endl;
	cout << "\t|---------------------------------------|" << endl;
	cout << endl;
	cout << endl;
	cout << "ViRTSA Application:" << endl;
	cout << "\t" << "- Init ViRTSA chai3d" << endl;
	cout << "\t" << "- TCP server for main application control" << endl;
	cout << "\t" << "- UDP sender of objects pos/rot" << endl;
	cout << endl;
	cout << endl;
	cout << "\t\t\t" << "(press [1] for help)" << endl;
	cout << endl;
	cout << endl;
}


int main(int argc, char **argv){
	TCPServer* ApplicationServer = new TCPServer();
	UDPSender* DataSender = new UDPSender();
	AppVirtsa* AppManager = new AppVirtsa();
	ApplicationServer->SetAppManager(AppManager);
	DataSender->SetAppManager(AppManager);

	// show welcome message
	welcome();

	// init TCP server
	if (ApplicationServer->Init()){
		cout << "-----------------------------------------" << endl
			<< " TRY MANUAL RECONNECTION - [3] TCP SERVER" << endl
			<< "-----------------------------------------" << endl;
	}

	// init UDP sender
	if (DataSender->Init()) {
		cout << "-----------------------------------------" << endl
			<< " TRY MANUAL CONNECTION - [5] UDP SENDER" << endl
			<< "-----------------------------------------" << endl;
	}

	// INTERFACE FOR DEBUG OR MANUAL MODIFICATIONS
	int command = -1;
	do{
		cin >> command;
		switch (command){
			case 1:		// show help
				help();
				break;
			case 2:		// stop TCP server
				ApplicationServer->Stop();
				break;
			case 3:		// TCP server (re)init
				cout << "-----------------------------------------" << endl
					<< " MANUAL RECONNECTION OF TCP SERVER" << endl
					<< "-----------------------------------------" << endl;
				switch (ApplicationServer->getState()){
					case TCP_INITIALIZED:
					case TCP_LISTENING:
					case TCP_CLIENT_CONNECTED:
						cout << "\tServer already initialized. STOP [2] before." << endl
							<< "-----------------------------------------" << endl;
						break;
					case TCP_CONNECTING_ERROR:
					case TCP_CLIENT_DISCONNECTED:
					case TCP_COMMUNICATION_ERROR:
						ApplicationServer->Stop();
					case TCP_NOT_INITIALIZED:
					case TCP_CONNECTION_CLOSED:
					default:
						cout << "Server TCP listening PORT = ";
						char newPort[6] = "";
						cin >> newPort;
						//cout << "newPort " << newPort << endl;
						ApplicationServer->SetListeningPort(newPort);
						cout << "-----------------------------------------" << endl;
						ApplicationServer->Init();
						break;
				}
				break;
			case 4:		// stop UDP sender
				DataSender->Stop();
				break;
			case 5:		// UDP sender (re)init
				cout << "-----------------------------------------" << endl
					<< " MANUAL CONNECTION OF UDP SENDER" << endl
					<< "-----------------------------------------" << endl;
				switch (DataSender->getState()){
					case UDP_INITIALIZED:
					case UDP_SENDING_DATAGRAMS:
						cout << "\tSender already working. STOP [4] before." << endl
							<< "-----------------------------------------" << endl;
						break;
					case UDP_COMMUNICATION_ERROR:
						DataSender->Stop();
					case UDP_NOT_INITIALIZED:
					case UDP_COMMUNICATION_ENDED:
					default:
						cout << "Receiver UDP listening IP = ";
						char newIP[16] = "";
						cin >> newIP;
						//cout << "newIP " << newIP << endl;
						DataSender->SetReceiverIP(newIP);
						cout << "Receiver UDP listening PORT = ";
						char newPort[6] = "";
						cin >> newPort;
						//cout << "newPort " << newPort << endl;
						DataSender->SetReceiverPort(newPort);
						cout << "-----------------------------------------" << endl;
						DataSender->Init();
						break;
				}
				break;
			case 6:		// send datagrams (UDP sender)
				DataSender->SetSendingState(true);
				break;
			case 7:		// pause datagrams (UDP sender)
				DataSender->SetSendingState(false);
				break;
			case 8: cout << " [8] > Show application state [TO DO]" << endl;
				break;
			default:
				break;
		}
	} while (command != 0);

	// shutdown and clean
	delete ApplicationServer;
	delete DataSender;

	// wait for key to close
	char anything; 
	cin >> anything;
}
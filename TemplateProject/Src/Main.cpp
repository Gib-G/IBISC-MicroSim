/*
* Code à mettre ici !
*
* Dépendances disponibles dans ce projet :
* - Oculus
* - Bullet
* - ODE
*/

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <winsock2.h>
#include <WS2tcpip.h>
#include "Packet_generated.h"

#pragma comment(lib, "Ws2_32.lib")

std::string getAddressString(struct sockaddr* address) {

	char buffer[256]{};

	if (address->sa_family == AF_INET) {
		inet_ntop(AF_INET, reinterpret_cast<struct sockaddr_in*>(address), buffer, sizeof buffer);
		return static_cast<std::string>(buffer);
	}

	inet_ntop(AF_INET6, reinterpret_cast<struct sockaddr_in6*>(address), buffer, sizeof buffer);
	return static_cast<std::string>(buffer);

}

int main(int argc, char** argv) {

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		std::cout << "WSAStartup failed\n";
		return -1;
	}

	char const* listeningPort = "9999";

	struct addrinfo hint {};
	struct addrinfo* serverInfo{ nullptr };

	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = IPPROTO_TCP;
	hint.ai_flags = AI_PASSIVE;

	int status{};
	if ((status = getaddrinfo(nullptr, listeningPort, &hint, &serverInfo)) != 0) {
		std::cout << "getaddrinfo error: " << gai_strerror(status) << "\n";
		freeaddrinfo(serverInfo);
		WSACleanup();
		return -1;
	}

	SOCKET listeningSocketFd{};
	struct addrinfo* info{ nullptr };
	for (info = serverInfo; info != nullptr; info = info->ai_next) {
		if ((listeningSocketFd = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) == -1) {
			std::cout << "Cannot create socket for provided address info. Winsock error code " << WSAGetLastError() << "\n";
			continue;
		}
		if (bind(listeningSocketFd, info->ai_addr, static_cast<int>(info->ai_addrlen)) == SOCKET_ERROR) {
			closesocket(listeningSocketFd);
			std::cout << "Failed to bind socket. Winsock error code " << WSAGetLastError() << "\n";
			continue;
		}
		break;
	}

	freeaddrinfo(serverInfo);

	if (info == nullptr) {
		std::cout << "Server initialization failed\n";
		WSACleanup();
		return -1;
	}

	std::cout << "Server waiting for a connection at address " << getAddressString(info->ai_addr) << ", port " << listeningPort << "\n";

	if (listen(listeningSocketFd, 1) == -1) {
		std::cout << "listen error. Winsock error code " << WSAGetLastError() << "\n";
		WSACleanup();
		return -1;
	}

	struct sockaddr_storage clientAddress {};
	int clientAddressSize{ sizeof clientAddress };
	SOCKET clientSocketFd{};
	std::string clientAddressString{};
	while (1) {

		if ((clientSocketFd = accept(listeningSocketFd, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddressSize)) == -1) {
			std::cout << "accept error. Winsock error code " << WSAGetLastError() << "\n";
			WSACleanup();
			return -1;
		}

		clientAddressString = getAddressString(reinterpret_cast<struct sockaddr*>(&clientAddress));
		std::cout << "Client " << clientAddressString << " connected!" << "\n";

		double x{0};
		flatbuffers::FlatBufferBuilder builder{ 1024 };
		flatbuffers::Offset<Network::Packet> packet{};
		uint8_t* message{ nullptr };
		while (1) {

			packet = Network::CreatePacket(builder, static_cast<float>(cos(x)));
			builder.Finish(packet);
			message = builder.GetBufferPointer();

			if (send(clientSocketFd, reinterpret_cast<char*>(message), builder.GetSize(), 0) == -1) {
				closesocket(clientSocketFd);
				std::cout << "Sending failed. Client " << clientAddressString << " probably disconnected" << "\n";
				break;
			}

			x += 0.08;
			// This is some stupid attempt at synchronizing the server with
			// Unity (targeting a fixed display rate of about 60 fps).
			std::this_thread::sleep_for(std::chrono::milliseconds(16));

		}

	}

	WSACleanup();
	return 0;

}
#pragma once
#pragma warning (disable:4996)
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <iostream>
#include <string>

using namespace std;

enum E_STATIC
{
	PORT = 54321,
	BUFF_SIZE = 1024,
};

typedef struct FILE_STOCK
{
	char fileName[100];
	int fileSize;
};

class Network
{
public :
	Network() 
	{
		if (0 != WSAStartup(MAKEWORD(2, 2), &wsa)) { cout << "WSA Start up Fail" << endl; return; }
		listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
		do
		{
			if (INVALID_SOCKET == listenSocket) { cout << "socket create fail" << endl; break; }
		} while (false);
		
	}

	~Network() {}
		
	void bind()
	{
		ZeroMemory(&serverAddress, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(E_STATIC::PORT);
		serverAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		if (SOCKET_ERROR == ::bind(listenSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)))	
		{ 
			cout << "bind fail" << endl; 
		}

		// listen
		if (SOCKET_ERROR == ::listen(listenSocket, SOMAXCONN))
		{

		}
	}

	void acceptWithReceive()
	{
		while (true)
		{
			int addrlen = sizeof(clientAddress);
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddress, &addrlen);
			if (INVALID_SOCKET == clientSocket)
			{
				cout << "Client Socket Accept Fail" << endl; 
				continue;
			}

			//cout << "Client Accept IP : " << inet_ntoa(clientAddress.sin_addr) << ", Port : " << ntohs(clientAddress.sin_port) << endl;

			int retVal = ::recv(clientSocket, (char*)buff, sizeof(char)*E_STATIC::BUFF_SIZE, 0);
			if (SOCKET_ERROR == retVal)
			{
				cout << "recv Fail" << endl;
				return;
			}

			memcpy(&fileStock, buff, sizeof(fileStock));
			FILE* fileHandle = fileOpen();
			while (0 < fileStock.fileSize)
			{
				retVal = ::recv(clientSocket, (char*)buff, sizeof(char)*E_STATIC::BUFF_SIZE, 0);
				if (SOCKET_ERROR == retVal)
				{
					cout << "recv Fail" << endl;
					break;
				}

				int len = (BUFF_SIZE < fileStock.fileSize) ? BUFF_SIZE : fileStock.fileSize;
				fwrite(buff, 1, len, fileHandle);
				fileStock.fileSize -= len;
			}
			fclose(fileHandle);
		}
	}

	FILE* fileOpen()
	{		
		return std::fopen(fileStock.fileName, "wb");
	}

private:
	WSADATA wsa;
	SOCKET listenSocket;
	SOCKADDR_IN serverAddress;

	SOCKET clientSocket;
	SOCKADDR_IN clientAddress;
	char buff[E_STATIC::BUFF_SIZE];
	FILE_STOCK fileStock;
};

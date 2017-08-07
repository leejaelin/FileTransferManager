#pragma once
#pragma warning (disable:4996)
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

static const char* IP = "127.0.0.1";
enum E_STATIC
{	
	PORT = 54321,
	BUFF_SIZE = 1024,
};

typedef struct FILE_STOCK
{
	char fileName[100];
	int fileSize;
}FILE_STOCK;

class Network
{
public:
	Network()
	{
		if (0 != WSAStartup(MAKEWORD(2, 2), &wsa)) { cout << "WSA Start up Fail" << endl; return; }
		socket = ::socket(AF_INET, SOCK_STREAM, 0);
		do
		{
			if (INVALID_SOCKET == socket) { cout << "socket create fail" << endl; break; }
		} while (false);

	}

	~Network() {}

	void connect()
	{
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(E_STATIC::PORT);
		inet_pton(AF_INET, IP, &serverAddress.sin_addr);
		int retVal = ::connect(socket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
		if (SOCKET_ERROR == retVal) 
		{
			cout << "socket Fail" << endl;
			return;
		}

		Send();
	}
	
	void Send()
	{
		FILE * fp = fileOpen();
		int retVal = ::send(socket, (char*)&fileStock, sizeof(fileStock), 0);
		if (SOCKET_ERROR == retVal)
		{
			cout << "socket Send Fail" << __LINE__ << endl;
			return;
		}
		
		int end = fileStock.fileSize;
		cout << ftell(fp) << endl;
		int readSize = 0;
		for( int i = fileStock.fileSize; 0 < fileStock.fileSize; fileStock.fileSize-= BUFF_SIZE)
		{
			int len = (fileStock.fileSize < BUFF_SIZE) ? fileStock.fileSize : BUFF_SIZE;
			ZeroMemory(buff, sizeof(buff));
			readSize = fread(buff, sizeof(char), len, fp);
			cout << ftell(fp) << endl;
			if ( end <= ftell(fp) && 0 >= readSize )
			{
			//	break;
			}
			::send(socket, buff, len, 0);
			if (SOCKET_ERROR == retVal)
			{
				cout << "socket Send Fail" << __LINE__ << endl;
				return;
			}
		}

		fclose(fp);
	}

	FILE* fileOpen()
	{
		const char* path = "Desert.jpg";
		ZeroMemory(&fileStock, sizeof(fileStock));
		strcat_s(fileStock.fileName, path);
		std::FILE * fp = std::fopen(path, "rb");
		fseek(fp, 0, SEEK_END);
		fileStock.fileSize = ftell(fp);

		cout << fileStock.fileSize << endl;

		fseek(fp, 0, 0);
		return fp;
	}

private:
	WSADATA wsa;
	SOCKET socket;
	SOCKADDR_IN serverAddress;
	char buff[E_STATIC::BUFF_SIZE];
	FILE_STOCK fileStock;
};

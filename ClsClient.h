#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Windows.h>
#include <conio.h>
#include <fstream>

#include <memory>

#include <iostream>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

class ClsClient
{
private:
	WSADATA ws;
	SOCKET s1;
	sockaddr_in sa1;
	int connect_port;

	int err;

	int startUp();
	int createSocket(SOCKET& s);
	int connectServer(SOCKET& s, sockaddr_in& sa);

	bool isEnd(string str);

	void initTransfer(SOCKET s);

	void sendSizeToServer(SOCKET& s, string msg);
	string getMsgFromServer(SOCKET s);

public:

	ClsClient(int port);

	void sendReceivMsg();

	~ClsClient();
};


#pragma once
#include "stdinc.h"
#include <fstream>
#include <set>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Windows.h>
#include <conio.h>
#include <string>
#include <time.h>

#pragma comment (lib, "Ws2_32.lib")

#using <System.dll>

using namespace System;
using namespace System::Net;
using namespace System::Net::Mail;
using namespace System::Net::Mime;
using namespace System::Threading;
using namespace System::ComponentModel;


class ClsSocket
{
private:
	int my_port;// = 555;
	WSADATA ws;
	SOCKET s1;
	sockaddr_in sa1;

	set <int> clients;

	int err;

	int startUp();
	int creteSocket(SOCKET& s);
	int socketBind(SOCKET& s, sockaddr_in& sa);

	void getSocketForSelect(fd_set& read_s);
	void addNonBlockSocket(SOCKET s);

	int sendMail(string str);

	int sendMSG(SOCKET s, string msg);
	void getMSG(set<int>::iterator& it, string str);
	int sendFileToClient(SOCKET s);

	bool isEnd(string & str);

	int sendSizeMsg(SOCKET s, int size);
	int getSizeMsg(SOCKET s);

	string getCmd(SOCKET s, int size);

	void sendClientsList(string str);

	int listenSend(SOCKET& s, sockaddr_in& sa);
	
public:
	
	ClsSocket(int port);

	void connectSocketsListen();

	~ClsSocket();
};


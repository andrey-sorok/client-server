#include "ClsClient.h"



int ClsClient::startUp()
{
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
	{
		err = WSAGetLastError();
	}
	return err;
}

int ClsClient::createSocket(SOCKET& s)
{
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		err = WSAGetLastError();
	}
	
	return err;
}

int ClsClient::connectServer(SOCKET & s, sockaddr_in & sa)
{
	HOSTENT * hname;
	hname = gethostbyname("localhost");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(connect_port);
	sa.sin_addr.S_un = ((in_addr *)hname->h_addr_list[0])->S_un;

	int result = connect(s, (sockaddr*)&sa, sizeof(sa));
	if (result != 0)
	{
		err = WSAGetLastError();
	}
	return err;
}

bool ClsClient::isEnd(string str)
{
	bool rtn = 0;

	if (str.length() <= 0)
		return rtn;

	for (int i = 0; i < str.length() - 1; ++i)
	{
		if (str[i] == '#' && str[i + 1] == '&')
		{
			str = str.substr(0, i);
			rtn = 1;
			break;
		}
	}

	return rtn;
}

void ClsClient::initTransfer(SOCKET s)
{
	string msg = "#&";
	int result = send(s, msg.c_str(), sizeof(msg), 0);

	Sleep(100);
}

void ClsClient::sendSizeToServer(SOCKET& s, string msg)
{
	msg += "#&";
	int result = send(s, msg.c_str(), sizeof(msg), 0);
	string tmp = "";

	char inbuff[4];
	do
	{
		int res = recv(s, inbuff, sizeof(inbuff), 0);
		tmp = inbuff;

	} while (isEnd(tmp) == 0);
}

string ClsClient::getMsgFromServer(SOCKET s)
{
	char inbuff[1024];
	int res = 0;

	res = recv(s1, inbuff, sizeof(inbuff), 0);
	
	string msg = "#&";	
	int result = send(s, msg.c_str(), sizeof(msg.c_str()), 0);

	msg = inbuff;
	return msg.substr(0, msg.length()-2);
}

ClsClient::ClsClient(int port): err(-1)
{
	connect_port = port;

	startUp();
	createSocket(s1);
	connectServer(s1, sa1);

	char inbuff[1024];
	int res = recv(s1, inbuff, sizeof(inbuff), 0);

	cout << inbuff << endl;

}

void ClsClient::sendReceivMsg()
{
	string exit = "";
	while (exit != "exit")
	{
		string cmd = "";
		cin >> cmd;

		if (cmd == "discon")
		{
			initTransfer(s1);
			
			string msg = "discon";
			string sz = to_string(msg.length());
			
			sendSizeToServer(s1, sz);
			sendSizeToServer(s1, msg);

			msg = getMsgFromServer(s1);

			cout << msg << endl;
			
			closesocket(s1);
		}

		if (cmd == "connect")
		{
			startUp();
			createSocket(s1);
			connectServer(s1, sa1);

			char inbuff[1024];
			int res = recv(s1, inbuff, sizeof(inbuff), 0);
		}

		if (cmd == "mail")
		{
			string msg = "#&";
			int result = send(s1, msg.c_str(), sizeof(msg), 0);

			Sleep(100);

			msg = "mail Hellow, it`s mail from VC";
			string sz = to_string(msg.length());
			sendSizeToServer(s1, sz);
			sendSizeToServer(s1, msg);

			char inbuff[20];
			int res = recv(s1, inbuff, sizeof(inbuff), 0);
			if (res > 0)
				{
					string str = inbuff;
					cout << str.c_str() << endl;
				}
		}

		if (cmd == "file")
		{
			string msg = "#&";
			int result = send(s1, msg.c_str(), sizeof(msg), 0);

			Sleep(100);

			string sz = to_string(cmd.length());

			sendSizeToServer(s1, sz);
			sendSizeToServer(s1, cmd);

			int size = atoi(getMsgFromServer(s1).c_str());
	
			fstream f;
			f.open("file.txt", fstream::out);
			--size;

			char inbuff[1024];

			do
			{
				int res = recv(s1, inbuff, size, 0);
				if (res > 0)
				{
					f.write(inbuff, res);
				}
				size -= res;
				
			} while (size > 0);

			f.close();
		}
	}
}


ClsClient::~ClsClient()
{
}

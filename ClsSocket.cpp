#include "ClsSocket.h"



int ClsSocket::startUp()
{
	if ((WSAStartup(MAKEWORD(2, 2), &ws) != 0))
	{
		err = WSAGetLastError();
	}
	return err;
}

int ClsSocket::creteSocket(SOCKET& s)
{
	s= socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		err = WSAGetLastError();
	}

	ioctlsocket(s, FIONBIO, 0);
	return err;
}

int ClsSocket::socketBind(SOCKET& s, sockaddr_in& sa)
{
	sa.sin_family = AF_INET;
	sa.sin_port = htons(my_port);
	sa.sin_addr.S_un.S_addr = INADDR_ANY;

	if ( bind(s, (sockaddr*)&sa, sizeof(sockaddr)) !=0)
	{
		err = WSAGetLastError();
	}
	return err;
}

void ClsSocket::getSocketForSelect(fd_set& read_s)
{
	for (auto it = clients.begin(); it != clients.end(); ++it)
	{
		FD_SET(*it, &read_s);
	}

}

void ClsSocket::addNonBlockSocket(SOCKET s)
{
	int s2  = (int)accept(s, 0, 0);
	if (s2 > 0)
	{
		ioctlsocket(s2, FIONBIO, 0);

		clients.insert(s2);

		cout << s2 << endl;

		string s = "you are connecting";
		send(s2, s.c_str(), sizeof(s), 0);
	}
	else
	{
		//err = WSAGetLastError();
		//cout << err << endl;
	}
}

int ClsSocket::sendMSG(SOCKET s, string msg)
{
	msg += "#&";
	send(s, msg.c_str(), sizeof(msg), 0);

	char buffer[4];
	int count = 0;
	string str = "";

	do
	{
		count = recv(s, buffer, sizeof(buffer), 0);
		str = buffer;

	} while (isEnd(str) == 0);

	
	return 0;
}

void ClsSocket::getMSG(set<int>::iterator& it, string str)
{
	SOCKET s = *it;

	int pos = (int)str.find(" ");
	if (pos != -1)
	{
		string cmd = str.substr(0, pos);

		if (cmd == "mail")
		{
			
			str = str.substr(pos+1, str.length());
			if (sendMail(str))
			{
				string response = "Messege is send#&";
				sendto(s, response.c_str(), sizeof(response), 0, (sockaddr*)&sa1, sizeof(sa1));
			}
		}
	}
	else
	{
		if (str == "file")
		{
			sendFileToClient(s);
		}

		if (str == "discon")
		{

			sendMSG(s, "Your are disconnect");

			closesocket(*it);
			clients.erase(it++);
		}
	}
}

int ClsSocket::sendMail(string str)
{
	SmtpClient ^Smtp = gcnew SmtpClient("smtp.mail.ru", 25);
	Smtp->Credentials = gcnew NetworkCredential("webdi@mail.ru", "fcgbhfyn");
	Smtp->EnableSsl = true;

	MailMessage ^Message = gcnew MailMessage();
	Message->From = gcnew MailAddress("webdi@mail.ru");
	Message->To->Add(gcnew MailAddress("andrey-sorok@yandex.ru"));
	Message->Subject = "Заголовок";

	System::String^ sysStr = gcnew System::String(str.c_str());
	Message->Body = sysStr;
	Smtp->Send(Message);
	return 1;
}

int ClsSocket::sendFileToClient(SOCKET s)
{
	fstream f;
	f.open("file.txt", fstream::in);
	f.seekp(0, ios::end);
	long size = (long)f.tellp();
	f.seekp(0, ios::beg);
	
	sendSizeMsg(s, size);
	char * buffer = new char[size];

	while (!f.eof()) 
	{
		f.read(buffer, size);
		if (f.is_open())	
		{
			send(s, buffer, size, 0);
		}
		else
			break;
	}
	f.close();

	return 0;

}

inline string strToBuf(char * tmp, int size)
{
	string msg = to_string(size);
	for (int i = 0; i < msg.size(); ++i)
	{
		tmp[i] = msg[i];
	}

	return msg;
}

/*
string waitInMsg(SOCKET s)
{
	char buffer[1024];
	int count = 0;
	string str = "";

	do
	{
		count = recv(s, buffer, sizeof(buffer), 0);
		str = buffer;

	} while (isEnd(str) == 0);

	return str;
}
*/

int ClsSocket::sendSizeMsg(SOCKET s, int size)
{
	char * tmp = new char[size];
	string msg = strToBuf(tmp, size);

	send(s, tmp, msg.size(), 0);
	
	char buffer[1024];
	int count = 0;
	string str = "";
	
	do
	{
		count = recv(s, buffer, sizeof(buffer), 0);
		str = buffer;

	} while (isEnd(str) == 0);

	return msg.size();
}

bool ClsSocket::isEnd(string & str)
{
	bool rtn = 0;

	if (str.length() <= 0)
		return rtn;

	for (int i = 0; i < str.length()-1; ++i)
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

int ClsSocket::getSizeMsg(SOCKET s)
{
	int count;

	char buffer[1024];
	string tmp = "";

	do
	{
		count = recv(s, buffer, sizeof(buffer), 0);
		tmp = buffer;

	} while (isEnd(tmp) == false);

	string msg = "#&";
	send(s, msg.c_str(), msg.size(), 0);

	return atoi(tmp.c_str());
}

string ClsSocket::getCmd(SOCKET s, int size)
{
	char * buffer = new char[size];
	int count = 0;
	string str = "";

	do
	{
		count = recv(s, buffer, sizeof(buffer), 0);
		string tmp = buffer;

		str += tmp.substr(0, count);

	} while (isEnd(str) == 0);

	string msg = "#&";
	send(s, msg.c_str(), msg.size(), 0);

	str = str.substr(0, size);

	return str;
}

void ClsSocket::sendClientsList(string str)
{

}

ClsSocket::ClsSocket(int port) : err(-1)
{
	my_port = port;

	startUp();
	creteSocket(s1);
	socketBind(s1, sa1);
}

void ClsSocket::connectSocketsListen()
{
	listenSend(s1, sa1);
}

int ClsSocket::listenSend(SOCKET& s, sockaddr_in& sa)
{
	timeval time_out;

	time_out.tv_sec = 0; 
	time_out.tv_usec = 500000;
	
	int lis = listen(s, 5);
	if (lis != 0)
	{
		err = WSAGetLastError();
	} 

	ioctlsocket(s, FIONBIO, 0);

	clients.clear();	
	int mx = 0;

	while (!_kbhit())
	{
		fd_set read_s;
		FD_ZERO(&read_s);
		FD_SET(s , &read_s);

		cout << clients.size() << endl;

		if (!clients.empty())
		{
			getSocketForSelect(read_s);
			mx = max(s, *max_element(clients.begin(), clients.end()));
		}
		else
			mx = s;
		
		
		select(int(mx + 1), &read_s, NULL, NULL, &time_out);

		if (FD_ISSET(s, &read_s))
		{
			cout << "add: " << s << endl;
			addNonBlockSocket(s);
		}

		auto it = clients.begin();
		while(it != clients.end())
		{
			if (FD_ISSET(*it, &read_s))
			{
				char buf[4];
				int count = recv(*it, buf, sizeof(buf), 0);
				string tmp = buf;
				if (tmp[0] == '#' && tmp[1] == '&')
				{
					int size = getSizeMsg(*it);

					if (size == 0)
						break;

					string str = getCmd(*it, size);
					if (str != "")
					{
						getMSG(it, str);
					}
				}
			}

			if (it == clients.end())
				break;
			else
				++it;
		}
	}
	return err;
}


ClsSocket::~ClsSocket()
{
}

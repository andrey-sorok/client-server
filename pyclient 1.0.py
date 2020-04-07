
import socket
from string import ascii_letters
import re
import time

class SockClient:
    port = 0
    host = ''

    sock = 0

    def __init__(self, p, h):
        self.port = p
        self.host = h
    
    def sockConnect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.host, self.port))

        msg = self.sock.recv(1024)
        print(msg)

    def initSeans(self):
        s = b'#&'
        self.sock.send(s)
        time.sleep(0.1)

    def sendLength(self, l):
        st = str(l) + '#&'
        st = bytes(st, 'UTF-8')

        self.sock.send(st)
        self.sock.recv(1024)

    def sendMSG(self, msg):
        self.sock.send(msg)
        self.sock.recv(1024)

    def sendMail(self):
        self.initSeans()
        s = b'mail Hellow, it`s mail from PYTHON#&'
        self.sendLength(len(s))
        self.sendMSG(s)

    def getFile(self):
        self.initSeans()
        
        cmd = b'file'
        self.sendLength(len(cmd))
        cmd = b'file' + b'#&'
        self.sendMSG(cmd)
        
        size = self.sock.recv(1024)
        f = open('file.txt', 'w')
        size = (int(size.decode())) -1

        cmd = b'#&'
        self.sock.send(cmd)

        while size > 0:
            data = self.sock.recv(size) 
            data = data.decode('unicode-escape')   
       
            f.write(data)
        
            size = size - len(data)

        f.close()

    def sendCom(self):
        while True:
            s = input()
            s = bytes(s, 'UTF-8')

            if s == b'file':
                self.getFile()
            else:    
                self.sendMail()
              


sc = SockClient(555, 'localhost')

sc.sockConnect()
sc.sendCom()    
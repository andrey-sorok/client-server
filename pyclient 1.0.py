
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

    def strOut(self, s):
        src = re.search(b'#&', s)
        rtn = s[0:src.regs[0][0]]
        return rtn.decode()
    
    def sockConnect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.host, self.port))

        msg = self.sock.recv(1024)
        msg  = self.strOut(msg)
   
        print(msg)

    def sockDiscon(self):
        self.initSeans()
        msg = b'discon'

        self.sendLength(len(msg))
        self.sendMSG(msg)

        msg = self.getMsg()
        msg = self.strOut(msg)
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
        msg += b'#&'
        self.sock.send(msg)
        self.sock.recv(1024)

    def getMsg(self):
        msg = self.sock.recv(1024)
        self.sock.send(b'#&')
        return msg

    def sendMail(self):
        self.initSeans()
        s = b'mail Hellow, it`s mail from PYTHON'
        self.sendLength(len(s))
        self.sendMSG(s)

    def getFile(self):
        self.initSeans()
        
        cmd = b'file'
        self.sendLength(len(cmd))
        cmd = b'file' + b'#&'
        self.sendMSG(cmd)
        
        size = self.getMsg()
        
        f = open('file.txt', 'w')
        size = (int(size.decode())) -1
   
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
            elif s == b'mail':   
                self.sendMail()
            elif s == b'discon':
                self.sockDiscon()   
            elif s == b'connect':
                self.sockConnect()
                        


sc = SockClient(555, 'localhost')

sc.sockConnect()
sc.sendCom()    
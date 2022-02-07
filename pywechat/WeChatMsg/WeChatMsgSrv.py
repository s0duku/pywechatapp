
from .WeChatTextRecv import WeChatTextRecv
import signal
import threading
import os

__GLOBAL_MSG_STOP_SRV__ = False

class WeChatMsgSrv(threading.Thread):

    def __init__(self):
        super(WeChatMsgSrv,self).__init__()
        self.stop_srv = False
        self.text_recv = WeChatTextRecv()
        self.msg_records = []
        self.msg_ptr = 0
    
    def run(self):
        while not __GLOBAL_MSG_STOP_SRV__:
            id,wxid,text = self.text_recv.recvText()
            if id:
                self.msg_records.append((id,wxid,text))

    def getMsg(self):
        if len(self.msg_records) > self.msg_ptr:
            msg = self.msg_records[self.msg_ptr]
            self.msg_ptr += 1
            return msg
        return None

def WeChatMsgSrvQuit(signum,frame):
    global __GLOBAL_MSG_STOP_SRV__
    __GLOBAL_MSG_STOP_SRV__ = True
    print("Msg Srv Stop")
    os._exit(0)


signal.signal(signal.SIGINT,WeChatMsgSrvQuit)
signal.signal(signal.SIGTERM,WeChatMsgSrvQuit)
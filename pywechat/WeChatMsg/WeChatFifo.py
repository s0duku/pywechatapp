
from ..PyWechatConf import PyWechatConf
import mmap
import win32event

class WeChatFifo:
    def __init__(self):
        self.shm = mmap.mmap(0,PyWechatConf.WECHAT_HELPER_FIFO_INTERNAL_BUF_LEN,PyWechatConf.WECHAT_HELPER_FIFO_INTERNAL_BUF_NAME)
        self.shm_mutex = win32event.OpenMutex(PyWechatConf.WIN32_MUTEX_ALL_ACCESS,0,PyWechatConf.WECHAT_HELPER_FIFO_INTERNAL_BUF_MUTEX_NAME)
        if(not self.shm_mutex):
            raise
    def sendText(self,wxid,text):
        win32event.WaitForSingleObject(self.shm_mutex,0xffffffff)
        self.shm.seek(0)
        self.shm.write(PyWechatConf.WECHAT_HELPER_FIFO_CMD_SEND)
        self.shm.write(wxid.encode('utf16')[2:])
        self.shm.write(b'\x00\x00')
        self.shm.seek(PyWechatConf.WECHAT_HELPER_FIFO_INTERNAL_CMD_BUF_LEN+PyWechatConf.WECHAT_HELPER_FIFO_INTERNAL_ID_BUF_LEN)
        self.shm.write(text.encode('utf16')[2:])
        self.shm.write(b'\x00\x00')
        win32event.ReleaseMutex(self.shm_mutex)

    def userInfo(self,wxid):
        win32event.WaitForSingleObject(self.shm_mutex,0xffffffff)
        self.shm.seek(0)
        self.shm.write(PyWechatConf.WECHAT_HELPER_FIFO_CMD_INFO)
        self.shm.write(wxid.encode('utf16')[2:])
        self.shm.write(b'\x00\x00')
        self.shm.seek(PyWechatConf.WECHAT_HELPER_FIFO_INTERNAL_CMD_BUF_LEN+PyWechatConf.WECHAT_HELPER_FIFO_INTERNAL_ID_BUF_LEN)
        self.shm.write(b'\x00\x00')
        win32event.ReleaseMutex(self.shm_mutex)

        user_wxid = ''
        user_nickname = ''
        user_note = ''

        for i in range(100):
            win32event.WaitForSingleObject(self.shm_mutex,0xffffffff)
            self.shm.seek(0)
            cmd = self.shm.read(PyWechatConf.WECHAT_HELPER_FIFO_INTERNAL_CMD_BUF_LEN)
            if cmd == PyWechatConf.WECHAT_HELPER_FIFO_CMD_INFO_RET:
                self.shm.seek(PyWechatConf.WECHAT_HELPER_FIFO_INTERNAL_CMD_BUF_LEN+PyWechatConf.WECHAT_HELPER_FIFO_INTERNAL_ID_BUF_LEN)
                user_wxid = self.shm.read(PyWechatConf.WECHAT_HELPER_FIFO_INFO_WXID_LEN)
                user_nickname = self.shm.read(PyWechatConf.WECHAT_HELPER_FIFO_INFO_NICKNAME_LEN)
                user_note = self.shm.read(PyWechatConf.WECHAT_HELPER_FIFO_INFO_NOTE_LEN)
                self.shm.seek(0)
                self.shm.write(PyWechatConf.WECHAT_HELPER_FIFO_CMD_NONE_RET)
                win32event.ReleaseMutex(self.shm_mutex)
                user_wxid = user_wxid[:user_wxid.index(b'\x00')].decode('utf8')
                user_nickname = user_nickname[:user_nickname.index(b'\x00')].decode('utf8')
                user_note = user_note[:user_note.index(b'\x00')].decode('utf8')
                break
            win32event.ReleaseMutex(self.shm_mutex)
        return user_wxid,user_nickname,user_note
            

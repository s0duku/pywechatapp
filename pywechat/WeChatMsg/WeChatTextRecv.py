from ..PyWechatConf import PyWechatConf
import mmap
import win32event

class WeChatTextRecv:
    def __init__(self):
        self.shm = mmap.mmap(0,PyWechatConf.WECHAT_HELPER_MSG_INTERNAL_BUF_LEN,PyWechatConf.WECHAT_HELPER_MSG_INTERNAL_BUF_NAME)
        self.shm_mutex = win32event.OpenMutex(PyWechatConf.WIN32_MUTEX_ALL_ACCESS,0,PyWechatConf.WECHAT_HELPER_MSG_INTERNAL_BUF_MUTEX_NAME)
        if(not self.shm_mutex):
            raise
    def recvText(self):
        win32event.WaitForSingleObject(self.shm_mutex,0xffffffff)
        self.shm.seek(0)
        id = self.shm.read(PyWechatConf.WECHAT_HELPER_MSG_INTERNAL_ID_BUF_LEN)
        wxid = self.shm.read(PyWechatConf.WECHAT_HELPER_MSG_INTERNAL_WXID_BUF_LEN)
        text = self.shm.read(PyWechatConf.WECHAT_HELPER_MSG_INTERNAL_TEXT_BUF_LEN)
        self.shm.seek(0)
        self.shm.write(b'\x00')
        self.shm.seek(PyWechatConf.WECHAT_HELPER_MSG_INTERNAL_ID_BUF_LEN)
        self.shm.write(b'\x00')
        self.shm.seek(PyWechatConf.WECHAT_HELPER_MSG_INTERNAL_ID_BUF_LEN + PyWechatConf.WECHAT_HELPER_MSG_INTERNAL_WXID_BUF_LEN)
        self.shm.write(b'\x00')
        win32event.ReleaseMutex(self.shm_mutex)
        id = id[:id.index(b'\x00')].decode('utf8')
        wxid = wxid[:wxid.index(b'\x00')].decode('utf8')
        text = text[:text.index(b'\x00')].decode('utf8')
        return id,wxid,text
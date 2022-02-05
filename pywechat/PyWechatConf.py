
class PyWechatConf:
    WECHAT_HELPER_MSG_INTERNAL_BUF_NAME = 'wechathelpermsg'
    WECHAT_HELPER_MSG_INTERNAL_BUF_MUTEX_NAME = 'wechathelpermsgmutex'
    WECHAT_HELPER_MSG_INTERNAL_ID_BUF_LEN = 1024
    WECHAT_HELPER_MSG_INTERNAL_WXID_BUF_LEN = 1024
    WECHAT_HELPER_MSG_INTERNAL_TEXT_BUF_LEN = 8192
    WECHAT_HELPER_MSG_INTERNAL_BUF_LEN = WECHAT_HELPER_MSG_INTERNAL_ID_BUF_LEN + WECHAT_HELPER_MSG_INTERNAL_WXID_BUF_LEN + WECHAT_HELPER_MSG_INTERNAL_TEXT_BUF_LEN

    WECHAT_HELPER_FIFO_INTERNAL_BUF_NAME = 'wechathelperfifo'
    WECHAT_HELPER_FIFO_INTERNAL_BUF_MUTEX_NAME = 'wechathelperfifomutex'
    WECHAT_HELPER_FIFO_INTERNAL_ID_BUF_LEN = 1024
    WECHAT_HELPER_FIFO_INTERNAL_TEXT_BUF_LEN = 8192
    WECHAT_HELPER_FIFO_INTERNAL_CMD_BUF_LEN = 1
    WECHAT_HELPER_FIFO_INTERNAL_BUF_LEN = WECHAT_HELPER_FIFO_INTERNAL_CMD_BUF_LEN + WECHAT_HELPER_FIFO_INTERNAL_ID_BUF_LEN + WECHAT_HELPER_FIFO_INTERNAL_TEXT_BUF_LEN

    WECHAT_HELPER_FIFO_INFO_WXID_LEN = 256
    WECHAT_HELPER_FIFO_INFO_NICKNAME_LEN = 256
    WECHAT_HELPER_FIFO_INFO_NOTE_LEN = 256

    WECHAT_HELPER_FIFO_INFO_USER_INFO_LEN = WECHAT_HELPER_FIFO_INFO_WXID_LEN + WECHAT_HELPER_FIFO_INFO_NICKNAME_LEN + WECHAT_HELPER_FIFO_INFO_NOTE_LEN

    WECHAT_HELPER_FIFO_CMD_NONE = b'\x00'
    WECHAT_HELPER_FIFO_CMD_NONE_RET = b'\x01'
    WECHAT_HELPER_FIFO_CMD_SEND = b'\x02'
    WECHAT_HELPER_FIFO_CMD_INFO = b'\x03'
    WECHAT_HELPER_FIFO_CMD_INFO_RET = b'\x04'
    WECHAT_HELPER_FIFO_CMD_QUIT = b'\x05'

    WIN32_STANDARD_RIGHTS_REQUIRED = 0xF0000
    WIN32_SYNCHRONIZE = 0x100000
    WIN32_MUTANT_QUERY_STATE = 0x1
    WIN32_MUTEX_ALL_ACCESS = WIN32_STANDARD_RIGHTS_REQUIRED | WIN32_SYNCHRONIZE | WIN32_MUTANT_QUERY_STATE
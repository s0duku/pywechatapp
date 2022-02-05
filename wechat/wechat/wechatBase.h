#pragma once
#include <Windows.h>
#define WECHAT_LATEST_VERSION

#define WECHAT_HELPER_MSG_MUTEX_NAME "wechathelpermsgmutex"
#define WECHAT_HELPER_MSG_PIPE_NAME "wechathelpermsg"

#define WECHAT_HELPER_FIFO_MUTEX_NAME "wechathelperfifomutex"
#define WECHAT_HELPER_FIFO_PIPE_NAME "wechathelperfifo"

// 内部保存发送者 id 缓冲区长度
#define WECHAT_HELPER_MSG_INTERNAL_ID_BUF_LEN 1024
// 内部保存发送者 文本 缓冲区长度
#define WECHAT_HELPER_MSG_INTERNAL_TEXT_BUF_LEN 8192


enum {
	WECHAT_HELPER_FIFO_CMD_NONE = 0,
	WECHAT_HELPER_FIFO_CMD_NONE_RET,
	WECHAT_HELPER_FIFO_CMD_SEND,
	WECHAT_HELPER_FIFO_CMD_INFO,
	WECHAT_HELPER_FIFO_CMD_INFO_RET,
	WECHAT_HELPER_FIFO_CMD_QUIT,
};

#ifdef WECHAT_LATEST_VERSION
#define WECHAT_MODULE_NAME L"WeChatWin.dll"
#define WECHAT_BASE_ADDR 0x5A5F0000
#define	WECHAT_OFFSET_GET_STH (0x5a68d680 - 0x5A5F0000) 
#define WECHAT_OFFSET_LOGOUT_CALL (0x5adeaef0 - 0x5A870000)
#define WECHAT_OFFSET_SEND_TEXT_MSG_CALL	0x444540

#define WECHAT_OFFSET_RECV_TEXT_MSG_CALL (0x620a8b29 - 0x61b00000)
#define WECHAT_OFFSET_RECV_TEXT_MSG_CONTINUE (0x620a8b31 - 0x61b00000)
#define WECHAT_OFFSET_RECV_TEXT_MSG_BRANCH (0x620a8bd0 - 0x61b00000)

#define WECHAT_OFFSET_USER_INFO_CALL_0 (0x62139df0 - 0x61b00000)
#define WECHAT_OFFSET_USER_INFO_CALL_1 (0x61b99780 - 0x61b00000)
#define WECHAT_OFFSET_USER_INFO_CALL_2 (0x61eade90 - 0x61b00000)

#endif

struct wechatIdText {
	wchar_t* data;
	int data_len;
	int data_len2;
	wchar_t* tmp;
	wechatIdText(const wchar_t* text) {
		data = (wchar_t*)text;
		data_len = wcslen(text);
		data_len2 = wcslen(text);
		tmp = NULL;
	}
};

struct wechatUserInternal {
	BYTE wxid[256];
	BYTE nickname[256];
	BYTE note[256];
};

DWORD wechat_base_module_handle();
DWORD wechat_base_process_handle();
char* UnicodeToUtf8(const wchar_t* unicode, char* szUtf8);
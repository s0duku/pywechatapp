#pragma once

#include "wechatBase.h"

struct wechatFifoInternal {
	BYTE cmd;
	union {
		struct {
			BYTE id[WECHAT_HELPER_MSG_INTERNAL_ID_BUF_LEN];
			BYTE text[WECHAT_HELPER_MSG_INTERNAL_TEXT_BUF_LEN];
		}send_text;
		struct {
			BYTE id[WECHAT_HELPER_MSG_INTERNAL_ID_BUF_LEN];
			wechatUserInternal info;
		}user_info;
	}arg;
};

DWORD wechat_helper_load_fifo_module();
DWORD wechat_helper_unload_fifo_module();
DWORD WINAPI wechat_helper_fifo_loop(LPVOID lpParamter);
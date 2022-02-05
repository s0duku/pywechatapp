#pragma once

#include "wechatBase.h"



DWORD wechat_helper_load_msg_module();
DWORD wechat_helper_unload_msg_module();
DWORD wechat_recv_text_msg_hook();
DWORD wechat_recv_text_msg_unhook();
DWORD wechat_send_text_msg(const WCHAR* wxid,const WCHAR* msg);
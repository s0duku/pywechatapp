#pragma once


#include "wechatBase.h"

DWORD wechat_user_info(const wchar_t* user_id, wechatUserInternal* info);
DWORD wechat_user_logout();

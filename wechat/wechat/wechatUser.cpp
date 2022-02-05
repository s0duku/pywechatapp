
#include "wechatUser.h"
#include <Windows.h>


static BYTE wechat_user_info_buf[0x500] = { 0 };

DWORD wechat_user_info(const wchar_t * user_id,wechatUserInternal* info) {
	BYTE buf[0x60] = { 0x0 };
	DWORD call0 = wechat_base_module_handle() + WECHAT_OFFSET_USER_INFO_CALL_0;
	DWORD call1 = wechat_base_module_handle() + WECHAT_OFFSET_USER_INFO_CALL_1;
	DWORD call2 = wechat_base_module_handle() + WECHAT_OFFSET_USER_INFO_CALL_2;
	wechatIdText user_wxid(user_id);
	DWORD tmp = 0x0;
	BYTE buf2[0x60] = { 0x0 };
	__asm {
		pushfd;
		pushad;
		push eax;
		mov eax, dword ptr ss : [ebp - 0x4] ;
		mov tmp, eax;
		pop eax;
		lea edi, wechat_user_info_buf;
		push edi;
		sub esp, 0x14;
		lea eax, user_wxid;
		mov ecx, esp;
		mov dword ptr ss : [ebp - 0x2C] , esp;
		push eax;
		call call0;
		mov byte ptr ss : [ebp - 0x4] , 0x1;
		call call1;
		mov byte ptr ss : [ebp - 0x4] , 0x0;
		call call2;
		mov eax, tmp;
		mov dword ptr ss : [ebp - 0x4] , eax;
		popad;
		popfd;
	};
	UnicodeToUtf8((wchar_t*)*(((DWORD*)wechat_user_info_buf) + 4), (char *)buf);
	UnicodeToUtf8((wchar_t*)*(((DWORD*)wechat_user_info_buf) + 4), (char*)buf2);
	UnicodeToUtf8((wchar_t*)*(((DWORD*)wechat_user_info_buf) + 4), (char*)info->wxid);
	UnicodeToUtf8((wchar_t*)*(((DWORD*)wechat_user_info_buf) + 22), (char*)info->note);
	UnicodeToUtf8((wchar_t*)*(((DWORD*)wechat_user_info_buf) + 27), (char*)info->nickname);
	return 0;
}

DWORD wechat_user_logout() {
	DWORD logout_func;
	DWORD getsth_func;
	DWORD base_addr = wechat_base_module_handle();
	if (!base_addr) {
		return 0;
	}
	logout_func = WECHAT_OFFSET_LOGOUT_CALL + base_addr;
	getsth_func = WECHAT_OFFSET_GET_STH + base_addr;
	__asm {
		pushad;
		push 0;
		call getsth_func;
		mov ecx, eax;
		call logout_func;
		popad;
	};
	return 0;
}

DWORD wechat_self_info() {
	return 0;
}
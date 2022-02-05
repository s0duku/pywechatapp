
#include "wechatBase.h"
#include <Windows.h>

static DWORD module_base_handle = 0x0;
static DWORD process_base_handle = 0x0;

DWORD wechat_base_module_handle() {
	if (!module_base_handle) {
		module_base_handle = (DWORD)GetModuleHandle(WECHAT_MODULE_NAME);
	}
	return module_base_handle;
}

DWORD wechat_base_process_handle() {
	if (!process_base_handle) {
		process_base_handle = (DWORD)GetCurrentProcess();
	}
	return process_base_handle;
}

char* UnicodeToUtf8(const wchar_t* unicode,char * szUtf8)
{
	if (!szUtf8) {
		return NULL;
	}
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
	return szUtf8;
}

#include "wechatMsg.h"
#include <Windows.h>

struct wechatMsgInternal {
	BYTE id[WECHAT_HELPER_MSG_INTERNAL_ID_BUF_LEN];
	BYTE wxid[WECHAT_HELPER_MSG_INTERNAL_ID_BUF_LEN];
	BYTE text[WECHAT_HELPER_MSG_INTERNAL_TEXT_BUF_LEN];
};


static BYTE wechat_recv_text_msg_byte_code[8] = { 0 };
static DWORD wechat_recv_text_msg_addr = 0x0;
static DWORD wechat_recv_text_msg_continune_addr = 0x0;
static DWORD wechat_recv_text_msg_branch_addr = 0x0;
static DWORD wechat_recv_text_msg_old_protect = 0x0;
static DWORD textStruct = 0x0;
static char* textRecvId = 0x0;
static int textRecvIdLen = 0x0;
static char* textRecvContent = 0x0;
static char* textRecvSender = 0x0;
static int textRecvContentLen = 0x0;
static HANDLE wechat_helper_msg_mutex = 0x0;
static HANDLE wechat_helper_msg_pipe = 0x0;
static LPVOID wechat_helper_msg_data = 0x0;
static DWORD numOfWrite = 0x0;
static char errbuf[1024] = { 0 };

DWORD wechat_helper_get_msg_pipe() {
	return (DWORD)wechat_helper_msg_pipe;
}

__declspec(naked) void wechat_recv_text_msg_internal(){
	// naked means no compiler build frame, such as 'push ebp' ...
	// 保存寄存器状态
	_asm {
		pushfd;
		pushad;
	};
	_asm {
		mov textStruct, edx;
	};
	// 提取文本消息内容，注意不要取到不可访问的内存，会让程序不稳定。
	if (textStruct < 0x0fffffff && textStruct) {
		if (*(DWORD*)textStruct && *(DWORD*)textStruct < 0x0fffffff) {
			textStruct = *(DWORD*)(*(DWORD*)textStruct);
			if (textStruct && textStruct < 0x0fffffff) {
				WaitForSingleObject(wechat_helper_msg_mutex, INFINITE);
				// unicode有两种编码，这里的是utf16,把双字节utf16转单字节utf8
				textRecvId = UnicodeToUtf8((wchar_t*)*(((DWORD*)textStruct) + 18), (char*)((wechatMsgInternal*)wechat_helper_msg_data)->id);
				textRecvSender = UnicodeToUtf8((wchar_t*)*(((DWORD*)textStruct) + 18), (char*)((wechatMsgInternal*)wechat_helper_msg_data)->wxid);
				textRecvContent = UnicodeToUtf8((wchar_t*)*(((DWORD*)textStruct) + 28), (char*)((wechatMsgInternal*)wechat_helper_msg_data)->text);
				textStruct = *(DWORD *)(textStruct + 0x170);
				if (textStruct != 0) {
					textRecvSender = UnicodeToUtf8((wchar_t*)textStruct, (char*)((wechatMsgInternal*)wechat_helper_msg_data)->wxid);
				}
				//MessageBoxA(NULL,(char*)((wechatMsgInternal*)wechat_helper_msg_data)->text,(char *)((wechatMsgInternal*)wechat_helper_msg_data)->id, MB_OK);
				ReleaseMutex(wechat_helper_msg_mutex);
			}
		}
	}
	__asm
	{
		popad;
		popfd;
		mov dword ptr ss : [esp + 0x18] , 0x0;
		jmp	wechat_recv_text_msg_continune_addr;
	};
}

DWORD wechat_recv_text_msg_hook() {
	// 获取 hook掉的原始代码
	ReadProcessMemory((HANDLE)wechat_base_process_handle(), (LPVOID)wechat_recv_text_msg_addr, wechat_recv_text_msg_byte_code, sizeof(wechat_recv_text_msg_byte_code), 0);
	// 准备 hook用的代码，jmp our_call
	BYTE patched_code[5] = { 0 };
	patched_code[0] = 0xE9;
	*(DWORD*)&patched_code[1] = (DWORD)wechat_recv_text_msg_internal - wechat_recv_text_msg_addr - 5;
	//char buf[1024] = { 0 };
	//wsprintfA(buf, "Function at %p,Patched At %p", wechat_recv_text_msg_internal,wechat_recv_text_msg_addr);
	//MessageBoxA(NULL, buf, "Test", MB_OK);
	//写入代码
	if (!WriteProcessMemory((HANDLE)wechat_base_process_handle(), (LPVOID)wechat_recv_text_msg_addr, patched_code, 5, 0)) {
		MessageBoxA(NULL, "Hook WriteProcess Failed", "Test", MB_OK);
		return 0;
	}
	return 1;
}

DWORD wechat_recv_text_msg_unhook() {
	if (wechat_recv_text_msg_byte_code[0] == 0) {
		// 如果我们还没hook过，那就忽略
		return 0;
	}
	// 把原来hook的代码重写回去
	if(!WriteProcessMemory((HANDLE)wechat_base_process_handle(), (LPVOID)wechat_recv_text_msg_addr, wechat_recv_text_msg_byte_code, sizeof(wechat_recv_text_msg_byte_code), 0)){
		MessageBoxA(NULL, "UnHook WriteProcess Failed", "Test", MB_OK);
		return 0;
	}
	return 1;
}

DWORD wechat_helper_load_msg_module() {
	// 获取微信 wechatwin.dll 模块基地址
	DWORD base_addr = wechat_base_module_handle();
	if (!base_addr) {
		MessageBoxA(NULL, "Can not Get Module Handle", "Test", MB_OK);
		return 0;
	}
	// 接受消息hook的代码地址
	wechat_recv_text_msg_addr = base_addr + WECHAT_OFFSET_RECV_TEXT_MSG_CALL;
	// hook执行完后正常流程的代码地址
	wechat_recv_text_msg_continune_addr = base_addr + WECHAT_OFFSET_RECV_TEXT_MSG_CONTINUE;

	// 打开接收文本消息缓冲区互斥锁
	wechat_helper_msg_mutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, WECHAT_HELPER_MSG_MUTEX_NAME);
	if (INVALID_HANDLE_VALUE == wechat_helper_msg_mutex || !wechat_helper_msg_mutex) {
		// 打开失败，就创建一个
		wechat_helper_msg_mutex = CreateMutexA(NULL, FALSE, WECHAT_HELPER_MSG_MUTEX_NAME);
		if (INVALID_HANDLE_VALUE == wechat_helper_msg_mutex || !wechat_helper_msg_mutex) {
			// 创建失败，报错
			wsprintfA(errbuf, "Can not Open Msg Mutex, error %d", GetLastError());
			MessageBoxA(NULL, errbuf, "Test", MB_OK);
			return 0;
		}
	}
	//打开接收文本消息缓冲区
	wechat_helper_msg_pipe = OpenFileMappingA(FILE_MAP_ALL_ACCESS, 0, WECHAT_HELPER_MSG_PIPE_NAME);
	if (INVALID_HANDLE_VALUE == wechat_helper_msg_pipe || !wechat_helper_msg_pipe) {
		//打开失败则创建
		wechat_helper_msg_pipe = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(wechatMsgInternal), WECHAT_HELPER_MSG_PIPE_NAME);
		if (INVALID_HANDLE_VALUE == wechat_helper_msg_pipe || !wechat_helper_msg_pipe) {
			//无法获取缓冲区，报错
			wsprintfA(errbuf, "Can not Open File Mapping, error %d", GetLastError());
			MessageBoxA(NULL, errbuf, "Test", MB_OK);
			return 0;
		}
	}
	//映射文本消息缓冲区
	wechat_helper_msg_data = MapViewOfFile(wechat_helper_msg_pipe, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!wechat_helper_msg_data) {
		//映射失败，报错
		wsprintfA(errbuf, "Can not Map View Of File, file handle %p , error %d", wechat_helper_msg_pipe,GetLastError());
		MessageBoxA(NULL, errbuf, "Test", MB_OK);
		return 0;
	}

	// hook 接收消息call
	if (!wechat_recv_text_msg_hook()) {
		// hook 失败直接返回
		return 0;
	}

	
	//wechat_recv_text_msg_branch_addr = base_addr + WECHAT_OFFSET_RECV_TEXT_MSG_BRANCH;
	//char buf[1024] = { 0 };
	//wsprintfA(buf, "hook address at %p, continue address at %p", wechat_recv_text_msg_addr,wechat_recv_text_msg_continune_addr);
	//MessageBoxA(NULL, buf, "Test", MB_OK); 

	return 1;
}

DWORD wechat_helper_unload_msg_module() {
	// 卸载消息模块
	if (wechat_helper_msg_data) {
		// 如果有消息缓冲区映射，先释放映射
		UnmapViewOfFile(wechat_helper_msg_data);
		wechat_helper_msg_data = 0x0;
	}
	if (INVALID_HANDLE_VALUE != wechat_helper_msg_pipe && wechat_helper_msg_pipe) {
		// 如果有消息缓冲区，释放缓冲区
		CloseHandle(wechat_helper_msg_pipe);
		wechat_helper_msg_pipe = INVALID_HANDLE_VALUE;
	}
	//卸载 hook 的call
	if (!wechat_recv_text_msg_unhook()) {
		return 0;
	}
	return 1;
}


DWORD wechat_send_text_msg(const WCHAR* wxid, const WCHAR* text) {
	DWORD base_addr = wechat_base_module_handle();
	char buf1[1024] = { 0 };
	char buf2[1024] = { 0 };
	wechatIdText send_wxid(wxid);
	wechatIdText send_text(text);
	if (!base_addr) {
		return 0;
	}
	DWORD sendtext_func = base_addr + WECHAT_OFFSET_SEND_TEXT_MSG_CALL;
	__asm {
		pushfd;
		pushad;
		push 1;
		lea eax, buf1;
		push eax;
		lea edi, send_text;
		push edi;
		lea edx, send_wxid;
		lea ecx, buf2;
		call sendtext_func;
		add esp, 0xc;
		popad;
		popfd;
	};
	return 1;
}




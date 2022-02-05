// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include "wechatUser.h"
#include "wechatMsg.h"
#include "wechatFifo.h"

BOOL reason_dispatch(DWORD ul_reason_for_call) {
    CHAR buf[1024] = { 0 };
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //wsprintfA(buf, "call address at %p", wechat_base_module_handle() + WECHAT_OFFSET_LOGOUT_CALL);
        //MessageBoxA(NULL, buf, "Test", MB_OK); 
        if (!wechat_helper_load_msg_module()) {
            return FALSE;
        }
        if (!wechat_helper_load_fifo_module()) {
            return FALSE;
        }

        CreateThread(NULL, 0, wechat_helper_fifo_loop, NULL, 0, NULL);
        wechatUserInternal info;
        
        //wsprintfA(buf, "call address at %p", wechat_user_info);
        //MessageBoxA(NULL, buf, "test", MB_OK);
        wechat_user_info(L"wxid_1gpd9apgrpv222", &info);
        //MessageBoxA(NULL, (char *)info.nickname, (char *)info.wxid, MB_OK);
        //wechat_send_text_msg(L"wxid_1gpd9apgrpv222", L"TTTTTTTTTTTTTTTTTTTTTTTTTT");
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (!wechat_helper_unload_msg_module()) {
            return FALSE;
        }
        if (!wechat_helper_unload_fifo_module()) {
            return FALSE;
        }
        break;
    }
    return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    reason_dispatch(ul_reason_for_call);
    //wechat_user_logout();
    return TRUE;
}



#include "wechatFifo.h"
#include "wechatUser.h"
#include "wechatMsg.h"

static HANDLE wechat_helper_fifo_mutex = 0x0;
static HANDLE wechat_helper_fifo_pipe = 0x0;
static LPVOID wechat_helper_fifo_data = 0x0;
static BOOL keep_fifo_loop = TRUE;

char errbuf[1024] = { 0 };

DWORD wechat_helper_fifo_send_quit() {
	WaitForSingleObject(wechat_helper_fifo_mutex, INFINITE);
	((wechatFifoInternal*)wechat_helper_fifo_data)->cmd = WECHAT_HELPER_FIFO_CMD_QUIT;
	ReleaseMutex(wechat_helper_fifo_mutex);
	return 1;
}

DWORD wechat_helper_load_fifo_module() {
	// �򿪽���fifo������������
	wechat_helper_fifo_mutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, WECHAT_HELPER_FIFO_MUTEX_NAME);
	if (INVALID_HANDLE_VALUE == wechat_helper_fifo_mutex || !wechat_helper_fifo_mutex) {
		// ��ʧ�ܣ��ʹ���һ��
		wechat_helper_fifo_mutex = CreateMutexA(NULL, FALSE, WECHAT_HELPER_FIFO_MUTEX_NAME);
		if (INVALID_HANDLE_VALUE == wechat_helper_fifo_mutex || !wechat_helper_fifo_mutex) {
			// ����ʧ�ܣ�����
			wsprintfA(errbuf, "Can not Open Fifo Mutex, error %d", GetLastError());
			MessageBoxA(NULL, errbuf, "Test", MB_OK);
			return 0;
		}
	}
	//�򿪽���fifo������
	wechat_helper_fifo_pipe = OpenFileMappingA(FILE_MAP_ALL_ACCESS, 0, WECHAT_HELPER_FIFO_PIPE_NAME);
	if (INVALID_HANDLE_VALUE == wechat_helper_fifo_pipe || !wechat_helper_fifo_pipe) {
		//��ʧ���򴴽�
		wechat_helper_fifo_pipe = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(wechatFifoInternal), WECHAT_HELPER_FIFO_PIPE_NAME);
		if (INVALID_HANDLE_VALUE == wechat_helper_fifo_pipe || !wechat_helper_fifo_pipe) {
			//�޷���ȡ������������
			wsprintfA(errbuf, "Can not Open Fifo Mapping, error %d", GetLastError());
			MessageBoxA(NULL, errbuf, "Test", MB_OK);
			return 0;
		}
	}
	//ӳ��fifo������
	wechat_helper_fifo_data = MapViewOfFile(wechat_helper_fifo_pipe, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!wechat_helper_fifo_data) {
		//ӳ��ʧ�ܣ�����
		wsprintfA(errbuf, "Can not Map View Of Fifo, file handle %p , error %d", wechat_helper_fifo_pipe, GetLastError());
		MessageBoxA(NULL, errbuf, "Test", MB_OK);
		return 0;
	}

	return 1;
}

DWORD wechat_helper_unload_fifo_module() {
	// ж����Ϣģ��
	wechat_helper_fifo_send_quit();
	Sleep(10);
	if (wechat_helper_fifo_data) {
		// �������Ϣ������ӳ�䣬���ͷ�ӳ��
		UnmapViewOfFile(wechat_helper_fifo_data);
		wechat_helper_fifo_data = 0x0;
	}
	if (INVALID_HANDLE_VALUE != wechat_helper_fifo_pipe && wechat_helper_fifo_pipe) {
		// �������Ϣ���������ͷŻ�����
		CloseHandle(wechat_helper_fifo_pipe);
		wechat_helper_fifo_pipe = INVALID_HANDLE_VALUE;
	}
	return 1;
}

DWORD wechat_helper_fifo_exec(wechatFifoInternal* fifo) {
	switch (fifo->cmd) {
	case WECHAT_HELPER_FIFO_CMD_SEND:
		wechat_send_text_msg((wchar_t*)fifo->arg.send_text.id, (wchar_t*)fifo->arg.send_text.text);
		fifo->cmd = WECHAT_HELPER_FIFO_CMD_NONE_RET;
		break;
	case WECHAT_HELPER_FIFO_CMD_INFO:
		//MessageBoxA(NULL, "FIFO INFO", "TEST", MB_OK);
		wechat_user_info((wchar_t*)fifo->arg.user_info.id, &fifo->arg.user_info.info);
		fifo->cmd = WECHAT_HELPER_FIFO_CMD_INFO_RET;
		break;
	case WECHAT_HELPER_FIFO_CMD_QUIT:
		keep_fifo_loop = FALSE;
		fifo->cmd = WECHAT_HELPER_FIFO_CMD_NONE_RET;
		break;
	default:
		break;
	}
	return 0;
}


DWORD WINAPI wechat_helper_fifo_loop(LPVOID lpParamter) {
	while (keep_fifo_loop) {
		DWORD res = WaitForSingleObject(wechat_helper_fifo_mutex, INFINITE);
		wechat_helper_fifo_exec((wechatFifoInternal*)wechat_helper_fifo_data);
		ReleaseMutex(wechat_helper_fifo_mutex);
	}
	return 1;
}
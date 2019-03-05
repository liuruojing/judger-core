// OnlineJudgeStart.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<windows.h>

int main()
{
	while (true)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		
		bool bol = CreateProcess(TEXT("C:\\OnlineJudge\\OnlineJudge\\OnlineJudge.exe"), NULL, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
		if (bol == FALSE)
		{
			printf("启动判题内核失败\n");
			system("pause");
			exit(0);
		}
		printf("判题内核启动成功 ...\n");
		WaitForSingleObject(pi.hProcess, INFINITE);        // 等待子进程退出 
		printf("判题内核意外崩溃，重启中...\n");
		CloseHandle(pi.hProcess);                          // 关闭句柄 
		CloseHandle(pi.hThread);
	}

    return 0;
}


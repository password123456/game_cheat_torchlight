/*
created by password123456
*/

#include <windows.h>
#include <wininet.h>
#include "resource.h"

#define			CAPTION_NAME			"Torchlight"

HWND hDlgMain;
HWND hWindow;
DWORD dwProcessId=0;
DWORD hLibModule;
DWORD dwPid = 0;
DWORD Textcheck;
DWORD CheatCheck;

WIN32_FIND_DATA FindDLLFile;
HANDLE hThread;
HANDLE hProcess;
HANDLE hFind;

HINSTANCE hInst;
HMODULE hKernel32;
LPVOID pLibRemote;

HDC hdc;
PAINTSTRUCT ps;

HBRUSH hBrush;



char Windowname[MAX_PATH];
char szDllname[MAX_PATH]={"torchlight.dll"};
char szCurrentPath[MAX_PATH];
char szDllFullname[MAX_PATH];

int InjectDll(HANDLE hProcess);

BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
HINSTANCE g_hInst;

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdParam,int nCmdShow)
{
    g_hInst=hInstance;
	WNDCLASS WndClass;
	WndClass.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));
	HWND hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, MainDlgProc);
	ShowWindow(hWnd, SW_SHOW);
   	MSG msg;
  	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (msg.message == WM_KEYDOWN)
		{
			SendMessage(hWnd, msg.message, msg.wParam, msg.lParam);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;

}

BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch (iMessage) 
	{
	case WM_RBUTTONDOWN:
		MessageBox(NULL,"http://www.playGameWithCheat.com\r\n\nsendmefuture@gmail.com", "Thank For Playing", MB_OK);
		return FALSE;

	case WM_INITDIALOG:
		SendMessage(hDlg,WM_SETICON,TRUE, (LPARAM)LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON)));
		hBrush = GetSysColorBrush(COLOR_MENU);
		Textcheck=2;
		InvalidateRect(hDlg,NULL,FALSE);
		return TRUE;

	case WM_CTLCOLORSTATIC:
		
		if ( Textcheck == 0 ) // ON
		{
			if((HWND)lParam == GetDlgItem(hDlg, IDC_STATIC_ON))
			{
				SetBkMode((HDC)wParam,TRANSPARENT);
				SetTextColor(hdc,RGB(255,0,0));
				return(LRESULT)hBrush;
			}
			if((HWND)lParam == GetDlgItem(hDlg, IDC_STATIC_OFF))
			{
				SetBkMode((HDC)wParam,TRANSPARENT);
				SetTextColor(hdc,RGB(0,0,0));
				return(LRESULT)hBrush;
			}
		}
	
		if ( Textcheck == 2) // ON / OFF 둘다 OFF(검은색)
		{
			if((HWND)lParam == GetDlgItem(hDlg, IDC_STATIC_ON))
			{
				SetBkMode((HDC)wParam,TRANSPARENT);
				SetTextColor(hdc,RGB(0,0,0));
				return(LRESULT)hBrush;
			}
			if((HWND)lParam == GetDlgItem(hDlg, IDC_STATIC_OFF))
			{
				SetBkMode((HDC)wParam,TRANSPARENT);
				SetTextColor(hdc,RGB(0,0,0));
				return(LRESULT)hBrush;
			}
		}
	
	case WM_KEYDOWN:
		if (GetAsyncKeyState(VK_F9) & 0X8000)
		{
			hWindow = ::FindWindow(NULL, CAPTION_NAME);
			if (!hWindow)
			{
				MessageBox(NULL,"Torchlight 가 실행되지 않았습니다.", NULL,MB_OK | MB_ICONWARNING);
				return 0;
			}
			GetWindowThreadProcessId (hWindow, &dwProcessId);
			
			GetCurrentDirectory(MAX_PATH, szCurrentPath);
			wsprintf(szDllFullname, "%s\\%s", szCurrentPath, szDllname);
			
			if (GetCurrentProcessId() == dwProcessId)
			{
				return TRUE;
			}
			hProcess = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|
				PROCESS_VM_WRITE|PROCESS_VM_READ, FALSE, dwProcessId);
			
			
			hFind = FindFirstFile(szDllFullname, &FindDLLFile);
			
			if (hFind == INVALID_HANDLE_VALUE) // 파일이 없을 경우
			{
				MessageBox(NULL,szDllFullname,"파일을 찾을 수 없습니다.",MB_OK);
				Textcheck=0;
			}
			else // 찾았을 경우 
			{
				InjectDll(hProcess);
				Textcheck=0;
				FindClose(hFind);
				InvalidateRect(hDlg,NULL,FALSE);
			}
			
			InvalidateRect(hDlg,NULL,FALSE);
			return FALSE;
		}   
		return FALSE;

	case WM_PAINT:
		hdc=BeginPaint(hDlg, &ps);
		EndPaint(hDlg, &ps);
		return FALSE;
		
	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{
		//case IDC_BUTTON_ON:
			return FALSE;
						
		case WM_DESTROY:
			DeleteObject(hBrush);
			PostQuitMessage(0);
			return FALSE;
			
		}
	}
	return FALSE;
}

int InjectDll(HANDLE hProcess)
{
	HMODULE hKernel32 = GetModuleHandle("Kernel32");
	
	pLibRemote = VirtualAllocEx(hProcess, NULL, sizeof(szDllFullname),MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, pLibRemote, (void*)szDllFullname,sizeof(szDllFullname), NULL);
		
	hThread = CreateRemoteThread(hProcess, NULL, 0, 
	(LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA")
		, pLibRemote, 0, NULL);
	
	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, &hLibModule);

	CloseHandle(hThread);
	VirtualFreeEx(hProcess, pLibRemote, sizeof(szDllFullname), MEM_RELEASE);
	
	return 0;
}


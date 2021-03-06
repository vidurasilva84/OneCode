/****************************** 模块头 ******************************\
模块名:  CppSendWM_COPYDATA.cpp
项目名:      CppSendWM_COPYDATA
版权 (c) Microsoft Corporation.

基于windows 消息 WM_COPYDATA 进程间通信(IPC) 是一种在本地机器上windows应用程序交换数据机制。

接受程序必须是一个windows应用程序。数据被传递必须不包含指针或者不能被应用程序接受的指向对象的引用。

当发送WM_COPYDATA消息时，引用数据不能被发送进程别的线程改变。 接受应用程序应该只考虑只读数据。

如果接受应用程序想要在SendMessage返回之后进入数据， 它必须拷贝数据到本地缓存。

这个代码例子示范了通过SendMessage（WM_COPYDATA）发送一个客户端数据结构（MY_STRUCT）到接受应用程序
如果数据结构传值失败，应用程序显示一个诊断错误代码。一个典型的错误代码是0x5（非法访问），它是由于用户
接口权限隔离导致的。用户接口权限隔离阻止进程发送被选择的窗口消息和其他一些用户进程APIs，这些用户进程拥有
比较高的完整性。 当接受程序（CppReceiveWM_COPYDATA）运行在一个比发送程序高的完整性时候，你将会看到一个
"SendMessage(WM_COPYDATA) failed w/err 0x00000005"错误信息。


This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes and Manifest Dependencies
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include "Resource.h"

// 使可视风格可用
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma endregion


// 携带数据的结构.
struct MY_STRUCT
{
    int Number;

    wchar_t Message[256];
};


//
//   函数: ReportError(LPWSTR, DWORD)
//
//   目的: 因为某些函数的失败，显示带错误信息的窗口D
//
//   参数:
//   * pszFunction - 执行错误的函数的名字
//   * dwError - Win32错误码. 它的默认值是调用线程的最后错误码
//
//   注意: 如果你没有显式的指定dwError参数 ReportError必须紧贴着失败的函数后面
//   这是确保调用线程的最后错误不被其他的在错误函数和ReportError之间的API调用覆盖
//
void ReportError(LPCWSTR pszFunction, DWORD dwError = GetLastError())
{
    wchar_t szMessage[200];
    if (-1 != swprintf_s(szMessage, ARRAYSIZE(szMessage), 
        L"%s failed w/err 0x%08lx", pszFunction, dwError))
    {
        MessageBox(NULL, szMessage, L"错误", MB_ICONERROR);
    }
}


//
//   函数: OnCommand(HWND, int, HWND, UINT)
//
//   目的: 处理WM_COMMAND 消息
//
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
    if (id == IDC_SENDMSG_BUTTON)
    {
        // 找到目标窗口句柄
        HWND hTargetWnd = FindWindow(NULL, L"CppReceiveWM_COPYDATA");
        if (hTargetWnd == NULL)
        {
            MessageBox(hWnd, L"不能发现 \"CppReceiveWM_COPYDATA\" 窗口", 
                L"错误", MB_ICONERROR);
            return;
        }

        // 准备发送带数据的COPYDATASTRUCT结构
        MY_STRUCT myStruct;

        BOOL fTranslated = FALSE;
        myStruct.Number = GetDlgItemInt(hWnd, IDC_NUMBER_EDIT, &fTranslated, TRUE);
        if (!fTranslated)
        {
            MessageBox(hWnd, L"Invalid value of Number!", L"Error", MB_ICONERROR);
            return;
        }

        GetDlgItemText(hWnd, IDC_MESSAGE_EDIT, myStruct.Message, 
            ARRAYSIZE(myStruct.Message));

        COPYDATASTRUCT cds;
        cds.cbData = sizeof(myStruct);
        cds.lpData = &myStruct;

        // 通过WM_COPYDATA 消息发送COPYDATASTRUCT结构到接受窗口
        // (应用程序必须用SendMessage代替 PostMessage 发送 WM_COPYDATA
		// 因为接受程序必须接受， 而这是有保证的)
        SendMessage(hTargetWnd, WM_COPYDATA, reinterpret_cast<WPARAM>(hWnd), 
            reinterpret_cast<LPARAM>(&cds));

        DWORD dwError = GetLastError();
        if (dwError != NO_ERROR)
        {
            ReportError(L"发送消息(WM_COPYDATA)", dwError);
        }
    }
}


//
//   函数: OnClose(HWND)
//
//   目的: 处理the WM_CLOSE消息
//
void OnClose(HWND hWnd)
{
    EndDialog(hWnd, 0);
}


//
//  函数: DialogProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  处理主对话框窗口消息
//
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // 在OnCommand中处理WM_COMMAND 消息
        HANDLE_MSG (hWnd, WM_COMMAND, OnCommand);

        // 在OnClose中处理WM_CLOSE消息
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;
    }
    return 0;
}


//
//  函数: wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
//
//  目的:  应用程序入口点
//
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow)
{
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
}
#include "Hook.hpp"
#include <Windows.h>
#include "time.h"
#include "string" 
#include <sstream>
#include <fstream>
#include <iostream>
#include "VtuberDelegate.hpp"
#include "View.hpp"
#include<thread>
#include"EventManager.hpp"
#include <tchar.h>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif

#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

static HHOOK hhkLowLevelKybd;

static HHOOK hhkLowLevelMs;

static int HookCode(DWORD code)
{
	/*
	Translate the return code from hook and 
	return the std::string rep of the the code.
	ex. 0x88 -> "[SHIFT]"
	caps = Caps lock on
	shift = Shift key pressed
	WinUser.h = define statments
	LINK = https://msdn.microsoft.com/en-us/library/dd375731(v=VS.85).aspx
	*/
	int key=-1;
	switch (code) // SWITCH ON INT
	{
	// Char keys for ASCI
	// No VM Def in header

	case 0x41:
		key = 0;//a
		break;
	case 0x42:
		key = 1; 
		break;
	case 0x43:
		key = 2; 
		break;
	case 0x44:
		key = 3; 
		break;
	case 0x45:
		key = 4;
		break;
	case 0x46:
		key = 5; 
		break;
	case 0x47:
		key = 6; 
		break;
	case 0x48:
		key = 7; 
		break;
	case 0x49:
		key =8;
		break;
	case 0x4A:
		key = 9;
		break;
	case 0x4B:
		key = 10; 
		break;
	case 0x4C:
		key = 11; 
		break;
	case 0x4D:
		key = 12; 
		break;
	case 0x4E:
		key = 13; 
		break;
	case 0x4F:
		key = 14; 
		break;
	case 0x50:
		key = 15; 
		break;
	case 0x51:
		key = 16; 
		break;
	case 0x52:
		key = 17; 
		break;
	case 0x53:
		key = 18; 
		break;
	case 0x54:
		key = 19; 
		break;
	case 0x55:
		key = 20; 
		break;
	case 0x56:
		key = 21; 
		break;
	case 0x57:
		key = 22; 
		break;
	case 0x58:
		key = 23; 
		break;
	case 0x59:
		key = 24; 
		break;
	case 0x5A:
		key = 25; //z
		break;
	// Num Keyboard
	case 0x30:
		key = 26; //0 key
		break;
	case 0x31:
		key = 27;// 1 key
		break;
	case 0x32:
		key = 28;// 2 key
		break;
	case 0x33:
		key = 29;// 3 key
		break;
	case 0x34:
		key = 30;
		break;
	case 0x35:
		key =31;
		break;
	case 0x36:
		key = 32;
		break;
	case 0x37:
		key = 33;
		break;
	case 0x38:
		key = 34; 
		break;
	case 0x39:
		key = 35;// 9 key
		break;

	case VK_SPACE:
 		key = 36; 
		break;
	case VK_LSHIFT:
		key = 37;
		break;
	case VK_LCONTROL:
		key = 38;
		break;

	case VK_F1:
		key = 39;
		break;
	case VK_F2:
		key = 40;
		break;
	case VK_F3:
		key = 41;
		break;
	case VK_F4:
		key = 42;
		break;
	case VK_F5:
		key = 43;
		break;
	case VK_F6:
		key = 44;
		break;
	case VK_F7:
		key = 45;
		break;
	case VK_F8:
		key = 46;
		break;
	case VK_F9:
		key = 47;
		break;
	case VK_F10:
		key = 48;
		break;
	case VK_F11:
		key = 49;
		break;
	case VK_F12:
		key = 50;
		break;
	case VK_UP:
		key = 51;
		break;
	case VK_DOWN:
		key = 52;
		break;
	case VK_RIGHT:
		key = 53;
		break;
	case VK_LEFT:
		key = 54;
		break;
	case VK_OEM_COMMA:
		key = 55;
		break;
	case VK_OEM_PERIOD:
		key = 56;
		break;
	case VK_OEM_4:
		key = 57;
		break;
	case VK_OEM_6:
		key = 58;
		break;
	case VK_RSHIFT:
		key = 59;
		break;
	case VK_RCONTROL:
		key = 60;
		break;
	default:
		key = -1;
		break;
	}
	return key;
}

LRESULT CALLBACK Hook::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	EventManager *eventManager =VtuberDelegate::GetInstance()->GetView()->GetEventManager();

	KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
	if (nCode == HC_ACTION) {
		if (wParam == WM_KEYUP) {
			int re =HookCode(p->vkCode);
			if (re>=0) {
				eventManager->KeyEventUp(re);
			}			
		}
		if (wParam == WM_KEYDOWN) {
			int re = HookCode(p->vkCode);
			if (re >= 0) {
				eventManager->KeyEventDown(re);
			}	
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK Hook::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	EventManager *eventManager =VtuberDelegate::GetInstance()->GetView()->GetEventManager();

	PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;

	if (nCode == HC_ACTION) { 
		switch (wParam) {
			case WM_LBUTTONUP: {
				eventManager->LeftButtonUp();
				break;
			} 
			case WM_LBUTTONDOWN: {
				eventManager->LeftButtonDown();
				break;
			} 
			case WM_RBUTTONDOWN: {
				eventManager->RightButtonDown();
				break;
			} 
			case WM_RBUTTONUP: {
				eventManager->RightButtonUp();
				break;
			}
	    }
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT Hook::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)

{
	switch (msg) {
		case WM_COMMAND:
			break;
		case WM_PAINT:
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_INPUT: {
			UINT dwSize = 0;
			GetRawInputData((HRAWINPUT)lParam, (UINT)RID_INPUT, NULL,
				&dwSize,
				sizeof(RAWINPUTHEADER)); 

			LPBYTE lpbBuffer = new BYTE[dwSize]; 
			GetRawInputData(
				(HRAWINPUT)lParam, (UINT)RID_INPUT, (LPVOID)lpbBuffer,
				(PUINT)&dwSize,
				(UINT)sizeof(RAWINPUTHEADER));

			RAWINPUT *raw = (RAWINPUT *)lpbBuffer;
			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				EventManager *eventManager =VtuberDelegate::GetInstance()
						->GetView()
						->GetEventManager();
				
				int xPosRelative = raw->data.mouse.lLastX;
				int yPosRelative = raw->data.mouse.lLastY;
				eventManager->SetRelativeMouse(xPosRelative,yPosRelative);

			}
			delete[] lpbBuffer;
			break;
		}
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Hook::Hook() {}

Hook::~Hook() {}

void Hook::Strat() {
	isExist = false;
	th = new std::thread(&Hook::Run, this);
	th->detach();
}

void Hook::Stop() {
	isExist = true;
	UnhookWindowsHookEx(hhkLowLevelKybd);
	UnhookWindowsHookEx(hhkLowLevelMs);
}

void Hook::Run() {

	hhkLowLevelKybd =SetWindowsHookEx(WH_KEYBOARD_LL, Hook::KeyboardHookProc, 0, 0);
	hhkLowLevelMs =SetWindowsHookEx(WH_MOUSE_LL, Hook::MouseHookProc, 0, 0);

	
	HINSTANCE hInst;
	hInst = GetModuleHandle(NULL); 
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC |
		    CS_NOCLOSE;       
	wcx.lpfnWndProc = WindowProc; 
	wcx.cbClsExtra = 0;           
	wcx.cbWndExtra = 0;           
	wcx.hInstance = hInst;        
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);   
	wcx.hbrBackground = (HBRUSH)WHITE_BRUSH;     
	wcx.lpszMenuName = NULL;                     
	wcx.lpszClassName = _T("BONGOHOOK");
	wcx.hIconSm = NULL;

	if (!RegisterClassEx(&wcx)) {
		printf("RegisterClassEx failed");
		return;
	}

	//set windows position
	int OSDleft = GetSystemMetrics(SM_CXSCREEN) / 2 - 300;
	int OSDTop = GetSystemMetrics(SM_CYSCREEN) / 2;

	m_hWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
			WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, 
		wcx.lpszClassName,
		NULL,
		WS_VISIBLE | WS_POPUP, 
		OSDleft,               
		OSDTop,                
		300,                   
		300,                  
		(HWND)NULL, 
		(HMENU)NULL,   
		hInst,        
		(LPVOID)NULL);

	if (!m_hWnd) {
		//fail to creat window;
		return;
	}

	//regist raw input device
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = m_hWnd;
	if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) {
		return;
	};

	//message loop
	BOOL bRet;
	MSG msg;
	while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0) {
		if (bRet == -1||isExist) {
			break;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

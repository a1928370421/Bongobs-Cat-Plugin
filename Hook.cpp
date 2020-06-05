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
		key = 11;
		break;
	case 0x44:
		key = 12;
		break;
	case 0x45:
		key = 8;
		break;
	case 0x51:
		key = 7;
		break;
	case 0x52:
		key = 9;
		break;
	case 0x53:
		key =13;
		break;
	case 0x57:
		key = 14;
		break;
	// Num Keyboard
	case 0x31:
		key = 0;
		break;
	case 0x32:
		key = 1;
		break;
	case 0x33:
		key = 2;
		break;
	case 0x34:
		key = 3;
		break;
	case 0x35:
		key =4;
		break;
	case 0x36:
		key = 5;
		break;
	case 0x37:
		key = 6;
		break;
	case VK_SPACE:
		key = 10;
		break;
	default:
		key = -1;
		break;
	}
	return key;
}

LRESULT CALLBACK Hook::HookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{

	KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
	if (nCode == HC_ACTION) {

		if (p->vkCode == VK_LSHIFT || p->vkCode == VK_RSHIFT) {

		}
		if (wParam == WM_KEYUP) {
			int re =HookCode(p->vkCode);
			if (re>=0) {
				VtuberDelegate::GetInstance()->GetView()->OnKeyUp(re);
			}			
		}
		if (wParam == WM_KEYDOWN) {
			int re = HookCode(p->vkCode);
			if (re >= 0) {
				VtuberDelegate::GetInstance()->GetView()->OnKeyDown(re);
			}	
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK Hook::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;
	if (nCode == HC_ACTION) { 
		switch (wParam) {
		case WM_LBUTTONUP: {
			VtuberDelegate::GetInstance()->GetView()->OnLButtonUp();
			break;
		} 
		case WM_LBUTTONDOWN: {
			VtuberDelegate::GetInstance()->GetView()->OnLButtonDown();
			break;
		} 
		case WM_RBUTTONDOWN: {
			VtuberDelegate::GetInstance()->GetView()->OnRButtonDown();
			break;
		} 
		case WM_RBUTTONUP: {
			VtuberDelegate::GetInstance()->GetView()->OnRButtonUp();
			break;
		}
	    }
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

Hook::Hook() {}

Hook::~Hook() {}

void Hook::Strat() {
	th = new std::thread(&Hook::Run, this);
}

void Hook::Stop() {
	isExist = true;
	UnhookWindowsHookEx(hhkLowLevelKybd);
	UnhookWindowsHookEx(hhkLowLevelMs);
}

void Hook::Run() {

	hhkLowLevelKybd =SetWindowsHookEx(WH_KEYBOARD_LL, Hook::HookProcedure, 0, 0);
	hhkLowLevelMs =SetWindowsHookEx(WH_MOUSE_LL, Hook::MouseHookProc, 0, 0);

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

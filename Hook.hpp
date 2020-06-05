#pragma once
#include <Windows.h>
#include<thread>

class Hook {
public:

	static LRESULT CALLBACK HookProcedure(int nCode, WPARAM wParam,LPARAM lParam);

	static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam,  LPARAM lParam);

	Hook();

	~Hook();

	void Strat();	

	void Stop();

private:
	void Run();

	bool isExist;

	std::thread *th;

};

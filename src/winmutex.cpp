#include "winmutex.h"

WinMutex::WinMutex(const wchar_t* mutexID)
{
	SetLastError(NO_ERROR);
	handle=CreateMutex(nullptr, false, mutexID);
	if(handle==nullptr) {
		state=State::Invalid;
	} else if(GetLastError() == ERROR_ALREADY_EXISTS) {
		state=State::AlreadyExists;
	} else {
		state=State::Created;
	}
}


WinMutex::~WinMutex()
{
	close();
}


WinMutex::State WinMutex::getState(void)const
{
	return state;
}


void WinMutex::close(void)
{
	/*
	if(handle) {
		CloseHandle(handle);
		handle=nullptr;
	}
	*/
}

/*
void WinMutex::release(void)
{
	ReleaseMutex(handle);
	close();
}
*/
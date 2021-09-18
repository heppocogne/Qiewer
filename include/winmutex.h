#ifndef WINMUTEX_H_INCLUDED
#define WINMUTEX_H_INCLUDED

#include <windows.h>

class WinMutex
{
		HANDLE handle;
	public:
		WinMutex(const wchar_t* mutexID);
		enum class State {
			Invalid,
			AlreadyExists,
			Created
		};
		State getState(void)const;
		void close(void);
		//void release(void);
		~WinMutex();
	private:
		State state;
};

#endif

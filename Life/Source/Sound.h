/////////////////////////////////////////////////////////////
// Sound.h
//
// Sound support
//
#pragma once

class CWaveSound 
{
public:
	CWaveSound();
	~CWaveSound();

	PlaySound(LPCTSTR pszSound, int priority);
};


DWORD playMIDIFile(HWND hWndNotify, LPSTR lpszMIDIFileName);

#include "CheckRevision.h"
#include <stdio.h>
#include "ver.h"

const char old[] = "war2mod.dll";
const char d[] = "war2mod.w2p";
const char d2[] = "ver-ix86-1.dll";

extern "C" __declspec(dllexport) BOOL __stdcall CheckRevision(LPCSTR lpszFileName1, LPCSTR lpszFileName2, LPCSTR lpszFileName3, LPCSTR lpszFormulaString, DWORD * lpdwVersion, DWORD * lpdwChecksum, LPSTR lpszExeInfoString)
{
	bool f2021 = true;
	char buf2021[] = "\x8b\xc8\x23\x45\x8\xf7\xd1\x23\x4d\xfc\xb\xc8\x89\x4d";
	for (int i = 0; i < 14; i++)
	{
		if ((*(char*)(VERSION_CHECK + i)) != buf2021[i])f2021 = false;
	}
	bool f2020 = true;
	char buf2020[] = "\x9b\xf6\xc1\x20\x74\x6\xd9\xeb\xdd\x5d\xf8\x9b\xc9\xc3";
	for (int i = 0; i < 14; i++)
	{
		if ((*(char*)(VERSION_CHECK + i)) != buf2020[i])f2020 = false;
	}
	if (f2020 || f2021)
	{
		*(byte*)DLL_STATUS = 1;
		remove(old);
		remove(d);
		CopyFile(d2, d, false);
		typedef void (*GW)();
		HMODULE dllHandle = GetModuleHandle(d);
		if (dllHandle != NULL)
		{
			MessageBox(GetForegroundWindow(),
				"Cannot load server war2mod together with singleplayer war2mod.\nPlease remove it from plugins folder.",
				"War2 Mod Loader", MB_OK);
			return FALSE;
		}
		dllHandle = LoadLibrary(d);
		if (dllHandle != NULL)
		{
			*(HMODULE*)DLL_POINTER = dllHandle;
			GW w2p = NULL;
			if (f2020)
				w2p = (GW)GetProcAddress(dllHandle, "_");
			if (f2021)
				w2p = (GW)GetProcAddress(dllHandle, "_g");
			if (w2p != NULL)
				w2p();
		}
	}
	return TRUE;
}
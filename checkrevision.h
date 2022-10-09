#include <Windows.h>

extern "C" __declspec(dllexport) BOOL __stdcall CheckRevision(LPCSTR lpszFileName1, LPCSTR lpszFileName2, LPCSTR lpszFileName3, LPCSTR lpszFormulaString, DWORD *lpdwVersion, DWORD *lpdwChecksum, LPSTR lpszExeInfoString);
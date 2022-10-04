// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>

int Main() {
    MessageBoxW(0, L"DLL Search Order Hijacking is present", L"DLL Search Order Hijacking", 0);
    return 1;
}
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        MessageBox(0, L"Hello From testlib!", L"Hello", MB_ICONINFORMATION);
    }

    return TRUE;
}
__declspec(dllexport) void DnsFreeConfigStructure() { Main(); }
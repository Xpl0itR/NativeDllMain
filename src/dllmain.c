// Copyright © 2025 Xpl0itR
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

VOID WINAPI DllProcessAttach(_In_ HMODULE hModule);

#ifndef USE_QUEUE_USER_APC
DWORD WINAPI ThreadStartRoutine(_In_ LPVOID lpParameter)
{
    DllProcessAttach((HMODULE)lpParameter);
    return 0UL;
}
#endif

BOOL APIENTRY DllMain(_In_ HMODULE hModule, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
    DisableThreadLibraryCalls(hModule);
    if (fdwReason != DLL_PROCESS_ATTACH)
        return TRUE;

#ifdef USE_QUEUE_USER_APC
    return QueueUserAPC((PAPCFUNC)DllProcessAttach, GetCurrentThread(), (ULONG_PTR)hModule) != 0;
#else
    HANDLE thread = CreateThread(NULL, 0, ThreadStartRoutine, hModule, 0, NULL);
    if (thread == NULL)
        return FALSE;

    CloseHandle(thread);
    return TRUE;
#endif
}
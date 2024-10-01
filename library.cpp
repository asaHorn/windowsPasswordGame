#include "library.h"

#include <iostream>

//boilerplate dll (I think. TBH this could be doing nothing, but I am not touching it now)
BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved){
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

//on load
// True for successful load, false for failure
BOOLEAN __stdcall InitializeChangeNotify(void) {
    return TRUE;
}

//called when user is attempting to change password
//return FALSE to deny change, TRUE to accept
//this function being called does not mean the change was successful
BOOLEAN __stdcall PasswordFilter(PUNICODE_STRING uname, PUNICODE_STRING legalName, PUNICODE_STRING password, BOOLEAN SetOperation){
    return TRUE;
}

//called to notify that a password change has successfully occurred
//return 0 for OK, other number for error code
NTSTATUS __stdcall PasswordChangeNotify(PUNICODE_STRING uname, ULONG RelativeId, PUNICODE_STRING password){
    return 0;
}
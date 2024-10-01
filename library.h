#ifndef REDTEAM_LIBRARY_H
#define REDTEAM_LIBRARY_H

#include <Windows.h>
#include <subauth.h>

__declspec(dllexport) BOOLEAN __stdcall InitializeChangeNotify(void);

__declspec(dllexport) NTSTATUS __stdcall PasswordChangeNotify(
        UNICODE_STRING *uname,
        ULONG RelativeId,
        PUNICODE_STRING password
);

__declspec(dllexport) BOOLEAN __stdcall PasswordFilter(
        PUNICODE_STRING uname,
        PUNICODE_STRING legalName,
        PUNICODE_STRING password,
        BOOLEAN SetOperation
);

#endif //REDTEAM_LIBRARY_H

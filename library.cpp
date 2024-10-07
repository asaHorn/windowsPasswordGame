//USAGE
//this library compiles into a dll file
//place this dll file in the system32 directory
//edit the regkey HKLM:\System\CurrentControlSet\Control\Lsa -> notificationPackages
//add the name of the dll you placed into sys32 (minus the .dll part)
//restart the box.
//the dll should now be visible in procexp loaded under LSASS.exe
//passwords should be saved in C:\Windows\temp\lsass.log

//
// Asa Horn
// aoh9470@rit.edu
//

#include <Windows.h>
#include <SubAuth.h>
#include <string>
//#include <iostream>

bool equals(const WCHAR* str1, const WCHAR* str2, size_t len){
    for (size_t i = 0; i < len; ++i) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
}


size_t find(const WCHAR* bigString, size_t bigString_len, const WCHAR* smallString, size_t smallString_len) {
    if (smallString_len == 0 || bigString_len < smallString_len) {
        return -1;
    }

    for (size_t i = 0; i <= bigString_len - smallString_len; ++i) {
        if (equals(&bigString[i], smallString, smallString_len)) {
            return i;
        }
    }
    return -1;
}

bool contains(const WCHAR* bigString, size_t bigString_len, const WCHAR* smallString, size_t smallString_len) {
    return find(bigString, bigString_len, smallString, smallString_len) != -1;
}

bool contains_any(const WCHAR* bigString, size_t bigString_len, const WCHAR* substrings[], size_t substrings_len[], size_t substrings_count) {
    for (size_t i = 0; i < substrings_count; ++i) {
        if (contains(bigString, bigString_len, substrings[i], substrings_len[i])) {
            return true;
        }
    }
    return false;
}

int* extractDigits(const WCHAR* str, size_t strLen, size_t& returnedDigitsLen){
    int* digits = (int*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 15*sizeof(int)); //15 because that is the max ## of digits (16 or more would fail r0)
    for(size_t i=0; i<strLen; ++i){
        if(isdigit(str[i])){
            //add the digit from the input string to the digits array. Also subtract ASCII 0 to conv to ascii > dec. Increment the index of the array.
            digits[returnedDigitsLen++] = str[i] - L'0';
        }
    }
    return digits;
}

void SendFeedbackToUser(int code) {
    HANDLE hPipe = CreateFile(
            "\\\\.\\pipe\\PasswordFilterPipe",
            GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
    );

    if (hPipe != INVALID_HANDLE_VALUE) {
        DWORD dwWritten;
        WriteFile(hPipe, &code, sizeof(code), &dwWritten, NULL);
        CloseHandle(hPipe);
    }
}


//RULE -- 0
//reject pass >= 16 chars
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkZero(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    return password->Length/2 >= 16;
}

//RULE -- 1
//reject pass where sum of digits != 6
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkOne(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    size_t retLength=0;
    int* digits = extractDigits(password->Buffer, password->Length/2, retLength);
    int sum=0;

    for(int i=0; i<retLength; ++i){
        sum += digits[i];
    }

    HeapFree(GetProcessHeap(), 0, digits);
    //if(sum!=6){std::cerr << sum;}//debug
    return sum != 6;
}

//RULE -- 2
//reject pass without redteam name
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkTwo(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    const WCHAR* teamNames[] = {
            L"aiden",
            L"andrew",
            L"asa",
            L"craig",
            L"ethan",
            L"garret",
            L"jenna",
            L"nick",
            L"nicholas",
            L"noah"
    };

    size_t substrings_len[] = {
            5,
            6,
            3,
            5,
            5,
            6,
            5,
            4,
            8,
            4
    };

    return !contains_any(lPassword->Buffer, lPassword->Length/2, teamNames, substrings_len, 9);;
}

//RULE -- 3
//reject pass contain('e')
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkThree(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    const WCHAR* e = L"e";
    return contains(lPassword->Buffer, lPassword->Length/2, e, 1);
}

//RULE -- 4
//reject pass w/ consecutive digits
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkFour(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    for(size_t i=0; i<(password->Length/2)-1; ++i){
        if(isdigit(password->Buffer[i]) && isdigit(password->Buffer[i+1])){
            return true;
        }
    }
    return false;
}

//RULE -- 5
//reject pass != 4 caps
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkFive(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    int numUpper = 0;
    for(size_t i = 0; i<password->Length/2; ++i){
        if(IsCharUpperW(password->Buffer[i])){
            numUpper++;
        }
    }
    return numUpper != 4;
}

//RULE -- 6
//reject pass wo/ "kinshasa"
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkSix(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    return !contains(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"kinshasa"), 8);
}

//RULE -- 7
//reject pass where prod of digits != 6
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkSeven(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    size_t retLength=0;
    int* digits = extractDigits(password->Buffer, password->Length/2, retLength);

    if(retLength < 2){
        //can't have a product of < two numbers
        HeapFree(GetProcessHeap(), 0, digits);
        return false;
    }

    //do the multiplication
    int prod=digits[0]*digits[1];
    for(size_t i=2; i<retLength; ++i){
        prod *= digits[i];
    }

    HeapFree(GetProcessHeap(), 0, digits);
    return prod != 6;
}

//RULE -- 8
//reject pass without proper noun caps
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkEight(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    //find the required proper noun's index
    size_t capIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"kinshasa"), 8);
    //if the all lower case string matches the unaltered case then the password has the proper noun un-capitalized
    if(capIndex != -1){if(password->Buffer[capIndex] == lPassword->Buffer[capIndex]){return true;}}
    //do the same check for the names, all names which contain an e or are too long can be skipped because of earlier rules
    size_t asaIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"asa"), 3);
    if(asaIndex != -1){if(password->Buffer[asaIndex] == lPassword->Buffer[asaIndex]){return true;}}
    size_t nickIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"nick"), 4);
    if(nickIndex != -1){if(password->Buffer[nickIndex] == lPassword->Buffer[nickIndex]){return true;}}
    size_t craigIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"craig"), 5);
    if(craigIndex != -1){if(password->Buffer[craigIndex] == lPassword->Buffer[craigIndex]){return true;}}
    size_t noahIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"noah"), 4);
    if(noahIndex != -1){if(password->Buffer[noahIndex] == lPassword->Buffer[noahIndex]){return true;}}

    return false;
}

//RULE -- 9
//reject pass w/ out of order digits
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkNine(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    size_t retLength=0;
    int* digits = extractDigits(password->Buffer, password->Length/2, retLength);

    if(retLength < 2){
        //can't have a product of < two numbers
        HeapFree(GetProcessHeap(), 0, digits);
        return false;
    }

    //do the check
    int lastDigit = digits[0];
    for(size_t i=1; i<retLength-1; ++i){
        if(lastDigit > digits[i]){
            HeapFree(GetProcessHeap(), 0, digits);
            return true;
        }
    }

    HeapFree(GetProcessHeap(), 0, digits);
    return false;
}

//RULE -- 10
//reject pass wo/ secret
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkTen(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    return !contains(password->Buffer, password->Length/2, reinterpret_cast<const WCHAR *>(L"Th3iR"), 5);
}






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
extern "C" __declspec(dllexport) BOOLEAN __stdcall InitializeChangeNotify(void) {
    return TRUE; //error handling / init is lame
}

//called when user is attempting to change password
//return FALSE to deny change, TRUE to accept
//this function being called does not mean the change was successful
extern "C" __declspec(dllexport) BOOLEAN __stdcall PasswordFilter(PUNICODE_STRING uname, PUNICODE_STRING legalName, PUNICODE_STRING password, BOOLEAN SetOperation){
    //setup
    HANDLE hHeap = NULL;
    hHeap = HeapCreate(0, 0, 0);
    if(hHeap == NULL){
        //std::cerr << "Could not create heap"
        return 1;
    }

    //configure lPassword (password lowercase, for case-insensitive checks)
    PUNICODE_STRING lPassword = (PUNICODE_STRING) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(PUNICODE_STRING));
    lPassword->Buffer = (wchar_t*) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(wchar_t)*password->Length/2);
    CopyMemory(lPassword->Buffer, password->Buffer, password->Length/2);
    lPassword->Length = password->Length;
    lPassword->MaximumLength = password->MaximumLength;
    CharLowerBuffW(lPassword->Buffer, lPassword->Length/2);

    //RULE -- 0
    if(checkZero(password, lPassword)){
        SendFeedbackToUser(0);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r0: len";
        //std::cerr << password->Length/2;
        return FALSE;
    }

    //RULE -- 1
    else if(checkOne(password, lPassword)){
        SendFeedbackToUser(1);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r1: 6 sum";
        return FALSE;
    }

    //RULE -- 2
    else if(checkTwo(password, lPassword)){
        SendFeedbackToUser(2);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r2: name";
        return FALSE;
    }

    //RULE -- 3
    else if(checkThree(password, lPassword)){
        SendFeedbackToUser(3);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r3: e";
        return FALSE;
    }

    //RULE -- 4
    else if(checkFour(password, lPassword)){
        SendFeedbackToUser(4);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r4: consec digits I";
        return FALSE;
    }

    //RULE -- 5
    else if(checkFive(password, lPassword)){
        SendFeedbackToUser(5);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r5: uppercase";
        return FALSE;
    }

    //RULE -- 6
    else if(checkSix(password, lPassword)){
        SendFeedbackToUser(6);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r6: DRoC";
        return FALSE;
    }

    //RULE -- 7
    else if(checkSeven(password, lPassword)){
        SendFeedbackToUser(7);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r7: 6 mult";
        return FALSE;
    }

    //RULE -- 8
    else if(checkEight(password, lPassword)){
        SendFeedbackToUser(8);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r8: proper caps";
        return FALSE;
    }

    //RULE -- 9
    else if(checkNine(password, lPassword)){
        SendFeedbackToUser(9);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r9: ascend";
        return FALSE;
    }

    //RULE -- 10
    else if(checkTen(password, lPassword)){
        SendFeedbackToUser(10);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
        //std::cout << "r10: passwd";
        return FALSE;
    }

    //cleanup
    HeapFree(hHeap, 0, lPassword->Buffer);
    HeapFree(hHeap, 0, lPassword);

    return TRUE;
}

//called to notify that a password change has successfully occurred
//return 0 for OK, other number for error code
extern "C" __declspec(dllexport) NTSTATUS __stdcall PasswordChangeNotify(PUNICODE_STRING uname, ULONG RelativeId, PUNICODE_STRING password){
    SendFeedbackToUser(20);
    return 0;
}

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

//Check if two substrings equal eachother
//in: str1 and str2 to compare
//in: len, the number of chars to compare
//out: TRUE if the two sequences are identical, FALSE otherwise
bool equals(const WCHAR* str1, const WCHAR* str2, size_t len){
    for (size_t i = 0; i < len; ++i) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
}

//Find a substring in a larger string
//in:bigString, the larger string to find inside of
//in: bigString_len, the length of bigstring
//in: smallString, the string to find
//in: smallString_len, the length of smallstring
//out: index of smallString[0] if smallString appears in bigString, -1 otherwise
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

//Check if a string appears in another string.
//in:bigString, the larger string to find inside of
//in: bigString_len, the length of bigstring
//in: smallString, the string to find
//in: smallString_len, the length of smallstring
//out: TRUE if smallString appears in bigString, FALSE otherwise
bool contains(const WCHAR* bigString, size_t bigString_len, const WCHAR* smallString, size_t smallString_len) {
    return find(bigString, bigString_len, smallString, smallString_len) != -1;
}

//Check if a string contains any value in array
//in:bigString, the larger string to find inside of
//in: bigString_len, the length of bigstring
//in: array substrings, an array of smallstrings to check for
//in: array substrings_len[], an array containing the sizes of the above strings in the same order
//in: substrings_count, the length of substrings
//out: TRUE if bigString contains any value in substrings at any position, FALSE otherwise
bool contains_any(const WCHAR* bigString, size_t bigString_len, const WCHAR* substrings[], size_t substrings_len[], size_t substrings_count) {
    for (size_t i = 0; i < substrings_count; ++i) {
        if (contains(bigString, bigString_len, substrings[i], substrings_len[i])) {
            return true;
        }
    }
    return false;
}

//Extract all of the digits in a string into an array of ints
//in: str, the string to search
//in: strLen, the length of str
//in: a pointer to returnedDigitsLen, used to store the length of the returned array
//out: an array of ints containing all of the digits of the string in order
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

//Send a reponse code via a named pipe to SERVICE.exe
//Used for creating message boxes for user feedback (LSASS making message boxes = BSOD)
//in: int code to send
//out: void
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
//reject pass >= 26 chars
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkOne(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    return password->Length/2 >= 26;
}

//RULE -- 1
//reject pass where sum of digits != 18
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkTwo(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    size_t retLength=0;
    int* digits = extractDigits(password->Buffer, password->Length/2, retLength);

    //count digits returned by digit extractor
    int sum=0;
    for(int i=0; i<retLength; ++i){
        sum += digits[i];
    }

    HeapFree(GetProcessHeap(), 0, digits);
    //if(sum!=6){std::cerr << sum;}//debug
    return sum != 18;
}

//RULE -- 2
//reject pass without redteam name
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkThree(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    const WCHAR* teamNames[] = {
            L"truman",
            L"eisenhower",
            L"kennedy",
            L"johnson",
            L"nixon",
            L"ford",
            L"carter",
            L"reagan",
            L"bush"

    };

    size_t substrings_len[] = {
            6,
            10,
            7,
            7,
            5,
            4,
            6,
            6,
            4
    };

    return !contains_any(lPassword->Buffer, lPassword->Length/2, teamNames, substrings_len, 5);;
}

//RULE -- 3
//reject pass contain('e')
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkFour(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    const WCHAR* e = L"e";
    return contains(lPassword->Buffer, lPassword->Length/2, e, 1);
}

//RULE -- 5
//reject pass wo/ "manama"
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkFive(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    return !contains(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"manama"), 8);
}

//RULE -- 6
//reject pass where number 3 is not present
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkSix(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    return !contains(password->Buffer, password->Length/2, reinterpret_cast<const WCHAR *>(L"3"), 1);
}

//RULE -- 8
//reject pass without proper noun caps
//in: password, the password & lPassword, the password all lowercase
//out: true to reject the password, false to accept it
bool checkSeven(PUNICODE_STRING password, PUNICODE_STRING lPassword){
    //Known issue: Second+ occurrences of a name are not checked. TBH I don't care tho

    //find the required proper noun's index
    size_t capIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"manama"), 8);
    //if the all lower case string matches the unaltered case then the password has the proper noun un-capitalized
    if(capIndex != -1){if(password->Buffer[capIndex] == lPassword->Buffer[capIndex]){return true;}}

    //do the same check for the names, all names which contain an e or are too long can be skipped because of earlier rules
    size_t asaIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"truman"), 3);
    if(asaIndex != -1){if(password->Buffer[asaIndex] == lPassword->Buffer[asaIndex]){return true;}}
    size_t nickIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"johnson"), 4);
    if(nickIndex != -1){if(password->Buffer[nickIndex] == lPassword->Buffer[nickIndex]){return true;}}
    size_t craigIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"nixon"), 5);
    if(craigIndex != -1){if(password->Buffer[craigIndex] == lPassword->Buffer[craigIndex]){return true;}}
    size_t aliceIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"ford"), 5);
    if(aliceIndex != -1){if(password->Buffer[craigIndex] == lPassword->Buffer[craigIndex]){return true;}}
    size_t noahIndex = find(lPassword->Buffer, lPassword->Length/2, reinterpret_cast<const WCHAR *>(L"bush"), 4);
    if(noahIndex != -1){if(password->Buffer[noahIndex] == lPassword->Buffer[noahIndex]){return true;}}

    return false;
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
    lPassword->Buffer = (wchar_t*) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(wchar_t)*password->Length);
    CopyMemory(lPassword->Buffer, password->Buffer, password->Length);
    lPassword->Length = password->Length;
    lPassword->MaximumLength = password->MaximumLength;
    CharLowerBuffW(lPassword->Buffer, lPassword->Length);

    //RULE -- 1
    if(checkOne(password, lPassword)){
        SendFeedbackToUser(0);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
//        std::cout << "r0: len";
//        std::cerr << password->Length/2;
        return FALSE;
    }

    //RULE -- 2
    else if(checkTwo(password, lPassword)){
        SendFeedbackToUser(1);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
//        std::cout << "r1: 6 sum";
        return FALSE;
    }

    //RULE -- 3
    else if(checkThree(password, lPassword)){
        SendFeedbackToUser(2);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
//        std::cout << "r2: name";
        return FALSE;
    }

    //RULE -- 4
    else if(checkFour(password, lPassword)){
        SendFeedbackToUser(3);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
//        std::cout << "r3: e";
        return FALSE;
    }

    //RULE -- 5
    else if(checkFive(password, lPassword)){
        SendFeedbackToUser(4);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
//        std::cout << "r4: consec digits I";
        return FALSE;
    }

    //RULE -- 6
    else if(checkSix(password, lPassword)){
        SendFeedbackToUser(5);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
//        std::cout << "r5: uppercase";
        return FALSE;
    }

    //RULE -- 7
    else if(checkSeven(password, lPassword)){
        SendFeedbackToUser(6);
        HeapFree(hHeap, 0, lPassword->Buffer);
        HeapFree(hHeap, 0, lPassword);
//        std::cout << "r6: DRoC";
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

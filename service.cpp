#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hPipe = CreateNamedPipe(
            "\\\\.\\pipe\\PasswordFilterPipe",
            PIPE_ACCESS_INBOUND,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            512, 512, 0, NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        MessageBox(
                NULL,                           // Handle to the owner window
                "Tell asa that his tool sucks",    // The message text
                "My tool is broke :(",          // title
                MB_OK | MB_ICONINFORMATION      // Display options
        );
        return -1;
    }

    while (true) {
        if (ConnectNamedPipe(hPipe, NULL) != FALSE) {
            int code;
            DWORD bytesRead;
            BOOL result = ReadFile(hPipe, &code, sizeof(code), &bytesRead, NULL);
            if (result && bytesRead == sizeof(code)) {
                // Handle the feedback (e.g., log it or display it)
                if(code==0) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 0/10 | COSTCUTTING:\n\r\n\rStorage space isn't free! All passwords must be under 15 chars to save space.",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==1) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 1/10 | LUCKY SIX:\n\r\n\rSix is a lucky number. To decrease breaches, digits in passwords must sum to 6.",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==2) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 2/10 | TEAM APPRECIATION:\n\r\n\rWe appreciate our red team. Your password must contain the first name of your favorite red teamer.",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==3) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 3/10 | ANTI-BRUTEFORCE:\n\r\n\rWe have run some numbers and almost every password contains the letter 'e.' In order to make harder to guess passwords, the letter 'e' is now banned.",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==4) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 4/10 | COUNTING:\n\r\n\rWe have noticed lots of numbers in passwords. Sometimes they are bigger numbers than our system can handle. Going forward keep all numbers in your password single-digit.",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==5) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 5/10 | BIG-LETTER:\n\r\n\rSome of the letters are too big for our system too. Someone said we need to let you have *some* upper case letters though. Going forward keep exactly 4 UPPERCASE LETTERS in your password.",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==6) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 6/10 | GEOGRAPHY QUIZ:\n\r\n\rAirport employees should know their geography. All passwords must contain the capitol city of the country of the day. Today's country of the day is:\n\r'the DRoC'",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==7) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 7/10 | LUCKY SIX II:\n\r\n\rSix is still a lucky number, but we are still getting hit. To decrease breaches, digits in passwords must multiply to 6 as well.",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==8) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 8/10 | BASIC GRAMMAR:\n\r\n\rProper grammar is important! All proper nouns (required by other rules) must have their first letter properly capitalized.",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==9) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 9/10 | CAPTCHA:\n\r\n\rWe need to limit automatic password changes. Computers are famously bad at counting, so to prove your humanity all digits in your password should appear in ascending order (eg, 1-2edt3iluvred4).",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==10) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 10/10 | HELPDESK AUTHORIZATION:\n\r\n\rAll password changes must be authorized by the helpdesk (who totally isn't redteam). Please contact re... I mean the helpdesk at ext. 100201 for a keyphrase to include in your password.",
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==20){
                    MessageBox(
                            NULL,
                            "Your password change request has been ACCEPTED.\n\rThank you for using BOEING Password Protection. \n\r\"Boeing: quality engineering, every time!\"",
                            "Message From BOEING PASSWORD PROTECTION",
                            MB_OK | MB_ICONINFORMATION
                    );
                }
            }
            DisconnectNamedPipe(hPipe);
        }
    }

    CloseHandle(hPipe);
}

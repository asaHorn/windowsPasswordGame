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
                if(code==0) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 0/10 | COSTCUTTING:\n\r\n\rStorage space isn't free! All passwords must be under 18 chars to save space.",
                            "Message From REDTEAM",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==1) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 1/10 | LUCKY SIX:\n\r\n\rSix is a lucky number. To decrease breaches, digits in passwords must sum to 7.",
                            "Message From REDTEAM",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==2) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 2/10 | TEAM APPRECIATION:\n\r\n\rWe appreciate our team. Your password must contain the first name of a member of the csuite Your options are:\nBob, Dylan, Alice, Ashley, or Mark.",
                            "Message From REDTEAM",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==3) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 3/10 | ANTI-BRUTEFORCE:\n\r\n\rWe have run some numbers and almost every password contains the letter 'e.' In order to make harder to guess passwords, the letter 'e' is now banned.",
                            "Message From REDTEAM",          // The tile of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==4) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 4/10 | COUNTING:\n\r\n\rWe have noticed lots of numbers in passwords. Sometimes they are bigger numbers than our system can handle. Going forward keep all numbers in your password single-digit.",
                            "Message From REDTEAM",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==5) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 5/10 | BIG-LETTER:\n\r\n\rSome of the letters are too big for our system too. Someone said we need to let you have *some* upper case letters though. Going forward keep exactly 4 UPPERCASE LETTERS in your password.",
                            "Message From REDTEAM",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==6) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 6/10 | GEOGRAPHY QUIZ:\n\r\n\rStudents should know their geography. All passwords must contain the capitol city of the country of the day. Today's country of the day is:\n\r'the DRoC'",
                            "Message From REDTEAM",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==7) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 7/10 | Math Homework:\n\r\n\r Help me with my math homework. ceil(sqrt( 5 * 7 - 19 + 2 *7)/2) Put the answer in your password", //ans is 3
                            "Message From REDTEAM",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==8) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 8/10 | BASIC GRAMMAR:\n\r\n\rProper grammar is important! All proper nouns (required by other rules) must have their first letter properly capitalized.",
                            "Message From REDTEAM",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==9) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 9/10 | CAPTCHA:\n\r\n\rWe need to limit automatic password changes. Computers are famously bad at counting, so to prove your humanity all digits in your password should appear in ascending order (eg, 1-2edt3iluvred4).",
                            "Message From REDTEAM",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==10) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 10/10 | HELPDESK AUTHORIZATION:\n\r\n\rAll password changes require authorization. Please include the password change password in your password. I can't just give you the password though.\nThe password is an animal. The animal is in Genus lynx. The animal is from North America. The Animal does not share a homeland with Nolan Dyck.\nTo get the password perform common letter to number password substitutions for any as and is, case insensitive",
                            "Message From REDTEAM",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==20){
                    MessageBox(
                            NULL,
                            "Your password change request has been ACCEPTED.\n\rThank you for using REDTEAM Password Manager. \n\r\"REDTEAM: quality passwords, every time!\"",
                            "Message From REDTEAM",
                            MB_OK | MB_ICONINFORMATION
                    );
                }
            }
            DisconnectNamedPipe(hPipe);
        }
    }

    CloseHandle(hPipe);
}

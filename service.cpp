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
                if(code==1) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 1/10 | COSTCUTTING:\n\r\n\rStorage space isn't free! All passwords must be under 26 chars to save space.",
                            "Message From the red on your street",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==2) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 2/10 | CRISIS REMEMBRANCE: \n\r\n\r We need to remember the last close call. Every password's sum of digits must match the sum of digits of the year the cuban missile crisis occured", //sum 2 18
                            "Message From the red on your street",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==3) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 3/10 | HISTORY LESSON:\n\r\n\rWorld leaders should know their history. Your password must include the name of your favorite cold war president (of the US)",
                            "Message From the red on your street",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==4) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 4/10 | ANTI-BRUTEFORCE:\n\r\n\rWe have run some numbers and have determined that communists always use 'e.' In order as such the communism containment committee has outlawed the use of this letter in all passwords.",
                            "Message From the red on your street",          // The tile of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==5) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 5/10 | GEOGRAPHY QUIZ:\n\r\n\rWorld leaders should know their geography! All passwords must contain the capitol city of the country of the day. Today's country of the day is:\n\r'Bahrain'", //Manama
                            "Message From the red on your street",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==6) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 6/7 | Math Homework:\n\r\n\r Help me with my math homework. ceil(sqrt( 5 * 7 - 19 + 2 *7)/2) Put the answer in your password", //ans is 3
                            "Message From the red on your street",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==7) {
                    MessageBox(
                            NULL,
                            "Your password change request has been DENIED.\n\rRULE 7/7 | BASIC GRAMMAR:\n\r\n\rProper grammar is important! All proper nouns (required by other rules) must have their first letter properly capitalized.",
                            "Message From the red on your street",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==20){
                    MessageBox(
                            NULL,
                            "Your password change request has been ACCEPTED.\n\rThank you for using REDTEAM Password Manager. \n\r\"REDTEAM: quality passwords, every time!\"",
                            "Message From the red on your street",
                            MB_OK | MB_ICONINFORMATION
                    );
                }
            }
            DisconnectNamedPipe(hPipe);
        }
    }

    CloseHandle(hPipe);
}

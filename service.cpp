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
                            "Your password change request has been DENIED.\n\rRED TEAM APPRECIATION:\n\r\n\rWe appreciate our red team. Please tell them that by including the exact phrase 'ThanksR3dTeam' in your passwords.",
                            "Message red team",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }
            }
            DisconnectNamedPipe(hPipe);
        }
    }

    CloseHandle(hPipe);
}

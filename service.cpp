#include <windows.h>
#include <stdio.h>

int main() {
    MessageBox(
        NULL,                  // Handle to the owner window (NULL for no owner)
        "This is a pop-up message.",  // The message text
        "Message Box Title",    // The title of the message box
        MB_OK | MB_ICONINFORMATION  // Display options (OK button, Information icon)
    );

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
                "bad pipe",    // The message text
                "note",          // title
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
                if(code==0){
                    MessageBox(
                            NULL,                           // Handle to the owner window (NULL for no owner)
                            "Your password change request has been DENIED.\n\rRULE 0/20 | COSTCUTTING:\n\r\n\rStorage space isn't free! All passwords must be under 20 chars to save space.",    // The message text
                            "Message From BOEING PASSWORD PROTECTION",          // The title of the message box
                            MB_HELP | MB_ICONHAND    // Display options (OK button, Information icon)
                    );
                }else if(code==20){
                    MessageBox(
                            NULL,                           // Handle to the owner window
                            "Your password change request has been ACCEPTED.\n\rThank you for using BOEING Password Protection. \n\r\"Boeing: quality engineering, every time!\"",    // The message text
                            "Message From BOEING PASSWORD PROTECTION",          // title
                            MB_OK | MB_ICONINFORMATION      // Display options
                    );
                }
            }
            DisconnectNamedPipe(hPipe);
        }
    }

    CloseHandle(hPipe);
}

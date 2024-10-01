#include <windows.h>
#include <stdio.h>

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;

void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);
void ListenForFeedback();

int main() {
    SERVICE_TABLE_ENTRY ServiceTable[] = {
            { "BOEINGPasswordDisplayService", (LPSERVICE_MAIN_FUNCTION)ServiceMain },
            { NULL, NULL }
    };

    StartServiceCtrlDispatcher(ServiceTable);
    return 0;
}

void ServiceMain(int argc, char** argv) {
    hStatus = RegisterServiceCtrlHandler("BOEINGPasswordDisplayService", (LPHANDLER_FUNCTION)ControlHandler);
    if (hStatus == (SERVICE_STATUS_HANDLE)0) {
        return;
    }

    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    SetServiceStatus(hStatus, &ServiceStatus);

    // Service initialization code here
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListenForFeedback, NULL, 0, NULL);
    MessageBox(
            NULL,                           // Handle to the owner window
            "started",    // The message text
            "note",          // title
            MB_OK | MB_ICONINFORMATION      // Display options
    );

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);

    // Service running code here
    while (ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
        Sleep(1000);
    }

    return;
}

void ControlHandler(DWORD request) {
    switch (request) {
        case SERVICE_CONTROL_STOP:
            ServiceStatus.dwWin32ExitCode = 0;
            ServiceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus(hStatus, &ServiceStatus);
            return;
        case SERVICE_CONTROL_SHUTDOWN:
            ServiceStatus.dwWin32ExitCode = 0;
            ServiceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus(hStatus, &ServiceStatus);
            return;
        default:
            break;
    }

    SetServiceStatus(hStatus, &ServiceStatus);
    return;
}

void ListenForFeedback() {
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
        return;
    }

    while (ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
        MessageBox(
                NULL,                           // Handle to the owner window
                "listening",    // The message text
                "note",          // title
                MB_OK | MB_ICONINFORMATION      // Display options
        );
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

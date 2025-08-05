#include <windows.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <io.h>
#include <chrono>
#include <locale>
#include <thread>
#include <vector>

#include "messageHandler.h"

#define VERSION L"1.0.1"
#define PIPE_NAME L"\\\\.\\pipe\\LUCID_MacroPipe"
#define BUFFER_SIZE 128

const std::wstring green = L"\033[32m";
const std::wstring red = L"\033[31m";
const std::wstring yellow = L"\033[33m";
const std::wstring cyan = L"\033[36m";
const std::wstring white = L"\033[0m";
const std::wstring bold = L"\033[1m";

void delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void printMessage(const std::wstring& message, const std::wstring& color) {
    std::wcout << color << bold;
    std::wcout << L"[" << message << L"]" << white << std::endl;
}

void printMessageWithDelay(const std::wstring& message, const std::wstring& color, int delay_ms) {
    std::wcout << color << bold;
    for (wchar_t c : message) {
        std::wcout << c << std::flush;
        delay(delay_ms);
    }
    std::wcout << white << std::endl;
}

void animateWaitingMessage(const std::wstring& message, const std::wstring& color, int duration_ms, int frame_delay_ms = 100) {
    std::wcout << color << bold << message << L" " << std::flush;
    std::wstring spinner = L"|/-\\";
    int num_frames = duration_ms / frame_delay_ms;

    for (int i = 0; i < num_frames; ++i) {
        std::wcout << L"\b" << spinner[i % spinner.length()] << std::flush;
        delay(frame_delay_ms);
    }
    std::wcout << L"\b " << white << std::endl;
}

void ErrorExit(const wchar_t* lpszFunction) {
    DWORD dwError = GetLastError();
    std::wcerr << red << bold << L"[FATAL_ERROR] " << lpszFunction << L" process failed with code " << dwError << L" (0x" << std::hex << dwError << L")" << white << std::endl;
}

std::wstring ReadNullTerminatedString(HANDLE hPipe) {
    std::wstring result;
    wchar_t buffer;
    DWORD bytesRead;

    while (true) {
        BOOL success = ReadFile(hPipe, &buffer, sizeof(wchar_t), &bytesRead, NULL);
        if (!success || bytesRead == 0) {
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                printMessage(L"WARNING > C# Server connection terminated unexpectedly. Data stream interrupted.", yellow);
            }
            else if (GetLastError() != ERROR_SUCCESS) {
                ErrorExit(L"ReadFile");
            }
            return L"";
        }

        if (buffer == L'\0') {
            return result;
        }
        result += buffer;
    }
}

HANDLE ConnectToServer() {
    HANDLE hPipe;

    printMessageWithDelay(L"INITIATING > P2P Connection Protocol...", cyan, 20);
    delay(500);

    while (true) {
        hPipe = CreateFile(
            PIPE_NAME,
            GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_PIPE_BUSY) {
            ErrorExit(L"CreateFile");
            return INVALID_HANDLE_VALUE;
        }

        printMessageWithDelay(L"STATUS > Target server busy. Retrying connection...", yellow, 20);
        delay(1000);
        if (!WaitNamedPipe(PIPE_NAME, 5000)) {
            printMessage(L"FATAL_ERROR > Connection attempt timed out. C# Server not responding.", red);
            delay(500);
            return INVALID_HANDLE_VALUE;
        }
    }

    printMessage(L"SUCCESS > Connection to C# Server established. Data link active.", green);
    delay(500);
    return hPipe;
}

void printIntro() {
    const std::wstring red = L"\033[31m";
    const std::wstring white = L"\033[0m";

    std::wcout << red;

    std::wcout << LR"(
                                 ██▓     █    ██  ▄████▄   ██▓▓█████▄                         
                                ▓██▒     ██  ▓██▒▒██▀ ▀█  ▓██▒▒██▀ ██▌                        
                                ▒██░    ▓██  ▒██░▒▓█    ▄ ▒██▒░██   █▌                        
                                ▒██░    ▓▓█  ░██░▒▓▓▄ ▄██▒░██░░▓█▄   ▌                        
                                ░██████▒▒▒█████▓ ▒ ▓███▀ ░░██░░▒████▓                         
                                ░ ▒░▓  ░░▒▓▒ ▒ ▒ ░ ░▒ ▒  ░░▓   ▒▒▓  ▒                         
                                ░ ░ ▒  ░░░▒░ ░ ░   ░  ▒    ▒ ░ ░ ▒  ▒                         
                                  ░ ░    ░░░ ░ ░ ░         ▒ ░ ░ ░  ░                         
                                    ░  ░   ░     ░ ░       ░     ░                            
                                                 ░             ░                              
                                 ███▄ ▄███▓ ▄▄▄       ▄████▄   ██▀███   ▒█████                
                                ▓██▒▀█▀ ██▒▒████▄    ▒██▀ ▀█  ▓██ ▒ ██▒▒██▒  ██▒              
                                ▓██    ▓██░▒██  ▀█▄  ▒▓█    ▄ ▓██ ░▄█ ▒▒██░  ██▒              
                                ▒██    ▒██ ░██▄▄▄▄██ ▒▓▓▄ ▄██▒▒██▀▀█▄  ▒██   ██░              
                                ▒██▒   ░██▒ ▓█   ▓██▒▒ ▓███▀ ░░██▓ ▒██▒░ ████▓▒░              
                                ░ ▒░   ░  ░ ▒▒   ▓▒█░░ ░▒ ▒  ░░ ▒▓ ░▒▓░░ ▒░▒░▒░               
                                ░  ░      ░  ▒   ▒▒ ░  ░  ▒     ░▒ ░ ▒░  ░ ▒ ▒░               
                                ░      ░     ░   ▒   ░          ░░   ░ ░ ░ ░ ▒                
                                       ░         ░  ░░ ░         ░         ░ ░                
                                                     ░                                        
                                  ██████▓██   ██▓  ██████ ▄▄▄█████▓▓█████  ███▄ ▄███▓  ██████ 
                                ▒██    ▒ ▒██  ██▒▒██    ▒ ▓  ██▒ ▓▒▓█   ▀ ▓██▒▀█▀ ██▒▒██    ▒ 
                                ░ ▓██▄    ▒██ ██░░ ▓██▄   ▒ ▓██░ ▒░▒███   ▓██    ▓██░░ ▓██▄   
                                  ▒   ██▒ ░ ▐██▓░  ▒   ██▒░ ▓██▓ ░ ▒▓█  ▄ ▒██    ▒██   ▒   ██▒
                                ▒██████▒▒ ░ ██▒▓░▒██████▒▒  ▒██▒ ░ ░▒████▒▒██▒   ░██▒▒██████▒▒
                                ▒ ▒▓▒ ▒ ░  ██▒▒▒ ▒ ▒▓▒ ▒ ░  ▒ ░░   ░░ ▒░ ░░ ▒░   ░  ░▒ ▒▓▒ ▒ ░
                                ░ ░▒  ░ ░▓██ ░▒░ ░ ░▒  ░ ░    ░     ░ ░  ░░  ░      ░░ ░▒  ░ ░
                                ░  ░  ░  ▒ ▒ ░░  ░  ░  ░    ░         ░   ░      ░   ░  ░  ░  
                                      ░  ░ ░           ░              ░  ░       ░         ░  
                                         ░ ░                                                  
)" << white << std::endl;
}

int main() {

    _setmode(_fileno(stdout), _O_WTEXT);

    _setmode(_fileno(stdin), _O_TEXT);

    std::locale::global(std::locale(""));

    message::processMessage("MESSAGE:You think you can hide from me ? Nah you can`t");


    printIntro();

    printMessageWithDelay(L"BOOT SEQUENCE > LUCID MacroCore v" VERSION L" initiated.", green, 5);
    printMessageWithDelay(L"INFO > CURRENT BUFFER SIZE : " + std::to_wstring(BUFFER_SIZE), green, 5);
    printMessageWithDelay(L"INFO > CURRENT PIPELINE : " PIPE_NAME L"\n\n", green, 5);
    printMessageWithDelay(L"SYSTEM STATUS > All core subroutines operational. \n\n", green, 5);

    HANDLE hPipe = ConnectToServer();
    if (hPipe == INVALID_HANDLE_VALUE) {
        printMessageWithDelay(L"CRITICAL ERROR > Server connection failure detected.", red, 5);
        delay(1000);
        printMessage(L"INITIATING > System shutdown sequence.", red);
        delay(500);
        return 1;
    }

    while (true) {
        animateWaitingMessage(L"Awaiting data transmission from server", cyan, 5);
        std::wstring response = ReadNullTerminatedString(hPipe);

        if (response.empty()) {
            printMessageWithDelay(L"FATAL > Server connection lost.", red, 5);
            delay(1000);
            printMessage(L"TERMINATING > Session disconnected.", red);
            delay(500);
            break;
        }

        std::wcout << green << L"COMMAND_RECEIVED > " << white << response << std::endl;
    }

    CloseHandle(hPipe);
    printMessageWithDelay(L"SESSION ENDED > Disconnecting all active protocols.", yellow, 5);
    delay(500);
    printMessage(L"STANDBY > System ready for next operation.", yellow);
    delay(500);

    return 0;
}

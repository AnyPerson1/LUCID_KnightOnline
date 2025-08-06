#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "message_handler.h"
#include <windows.h>
#include <fileapi.h>
#include "macro_management.h";
#include <winternl.h>
#pragma comment(lib, "ntdll.lib")


#if !DEBUG_STATE
void message::CheckDebuggerAndTriggerPanic()
{
	BOOL bDebuggerPresent = false;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &bDebuggerPresent);

	PROCESS_BASIC_INFORMATION pbi = { 0 };
	ULONG dwReturnLength = 0;

	typedef LONG(WINAPI* pfnNtQueryInformationProcess)(
		IN HANDLE ProcessHandle,
		IN PROCESSINFOCLASS ProcessInformationClass,
		OUT PVOID ProcessInformation,
		IN ULONG ProcessInformationLength,
		OUT PULONG ReturnLength OPTIONAL
		);

	pfnNtQueryInformationProcess NtQueryInfoProcess = (pfnNtQueryInformationProcess)GetProcAddress(
		GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");

	if (NtQueryInfoProcess) {
		NTSTATUS status = NtQueryInfoProcess(
			GetCurrentProcess(),
			ProcessBasicInformation,
			&pbi,
			sizeof(pbi),
			&dwReturnLength
		);

		if (NT_SUCCESS(status)) {
			PPEB pPeb = pbi.PebBaseAddress;
			if (pPeb && pPeb->BeingDebugged) {
				panic();
			}
		}
	}

	if (IsDebuggerPresent() || bDebuggerPresent)
	{
		panic();
	}

	DWORD error = GetLastError();
	OutputDebugStringA("nice try...");
	if (GetLastError() != error) {
		panic();
	}

	return;
}

void panic()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	while (1)
		MessageBoxW(NULL,
			L"Hey, merhaba...\n"
			L"Uygulamayı debug etmeye çalıştığını fark ettik.\n\n"
			L"Bu tür işlemler sistem yöneticileri tarafından izlenmektedir.\n"
			L"Her zaman güvenli ve yasal yolları tercih etmeni öneririz.\n\n"
			L"Bu, sadece bilgilendirme amaçlı bir uyarıdır.\n"
			L"Her girişiminizde daha derin önlemler alınmaktadır (memory corruption).\n\n"
			L"Bol şans! 😊\n"
			L"LUCID_KERNEL_MODULE",
			L"LUCID_KERNEL_MODULE :: Bilgilendirme",
			MB_ICONINFORMATION | MB_OK);
}

#endif

const std::wstring green = L"\033[32m";
const std::wstring red = L"\033[31m";
const std::wstring yellow = L"\033[33m";
const std::wstring cyan = L"\033[36m";
const std::wstring white = L"\033[0m";
const std::wstring bold = L"\033[1m";

bool enableDebugControl = false;

macro macro_instance;

void message::processMessage(std::string message)
{
	std::vector<std::string> sprt;

	{
		std::stringstream stream(message);
		std::string tmp_i;

		while (std::getline(stream,tmp_i, ':'))
		{
			sprt.push_back(tmp_i);
		}
	}
	
	classificate(sprt);
	
}


void message::classificate(const std::vector<std::string> &data)
{
	std::string header = data.at(0);
	std::string message = data.at(1);

	if (header == "MACRO_ACTIVATE")
	{
	}
	if (header == "MACRO_DEACTIVATE")
	{
	}
	if (header == "MACRO_INFO")
	{
		macro_instance.import_macro(data);
		return;
	}
	if (header == "MESSAGE")
	{
		std::string msgHeader = R"(INFO > THIS IS A MESSAGE SENT BY SYSTEM.
INCOMING MESSAGE:

)";
		std::string fullMsg = msgHeader + message;

		MessageBoxA(NULL, fullMsg.c_str(), "System", MB_OK | MB_ICONINFORMATION);
	}
}

void message::logMessage(std::string header, std::string message, std::wstring color)
{
	std::wcout << color;
	std::wcout << header.c_str() << " > " << message.c_str() << "\n";
}
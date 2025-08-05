#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "messageHandler.h"
#include <windows.h>
#include <fileapi.h>

const std::wstring green = L"\033[32m";
const std::wstring red = L"\033[31m";
const std::wstring yellow = L"\033[33m";
const std::wstring cyan = L"\033[36m";
const std::wstring white = L"\033[0m";
const std::wstring bold = L"\033[1m";

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
	
	std::wcout << "INFO > MESSAGE PROCESSING SUCCESS";
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

	}
	if (header == "PANIC")
	{
		while (1)
		MessageBoxA(NULL, "Panic mode activated. Starting memory corruption.", "System Error", MB_ICONERROR | MB_OK);
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
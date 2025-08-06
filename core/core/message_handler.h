#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#define DEBUG_STATE 1

class message
{
public:
	static void processMessage(std::string message);
#if !DEBUG_STATE
public:
	static void CheckDebuggerAndTriggerPanic();
#endif
private:
	static void classificate(const std::vector<std::string> &data);
public:
	static void logMessage(std::string header, std::string message, std::wstring color);
};

void panic();

#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

class message
{
public:
	static void processMessage(std::string message);

private:
	static void classificate(const std::vector<std::string> &data);
public:
	static void logMessage(std::string header, std::string message, std::wstring color);
};

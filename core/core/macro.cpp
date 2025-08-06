#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include <functional>
#include <sstream>
#include "macro.h"
#include "messageHandler.h"

#define ID_HEADER "ID"
#define MS_HEADER "MS"
#define CP_HEADER "CLICK_POINT"
#define SSCAN_AREA_HEADER "SCAN_RECT"
#define KEY_HEADER "HOTKEY"

const std::wstring green = L"\033[32m";
const std::wstring red = L"\033[31m";
const std::wstring yellow = L"\033[33m";
const std::wstring cyan = L"\033[36m";
const std::wstring white = L"\033[0m";
const std::wstring bold = L"\033[1m";

struct Point {
public:
	int x;
	int y;
};

struct Rectangle {
public:
	int x, y;
	int width, height;
};

class m_item {
public:
	std::string id;
	std::optional<int> ms;
	std::optional<Point> click_point;
	std::optional<Rectangle> screen_scan_area;
	std::optional<std::string> key;
};

std::vector<m_item> global_macros;

void macro::import_macro(std::vector<std::string> parts) // MACRO ITEM RECEIVE SAMPLE : ID:macroHeader:MS:macroSpeed:CLICK_POINT:clickPosition.x,clickPosition.y:SCAN_RECT:scanArea.x,scanArea.y,scanArea.width,scanArea.height:HOTKEY:macroHotkey
{
	m_item macro_item;
	std::string tmp_value;
	bool aborted = false;
	std::function<std::string(int)> getValue = [&parts](int i) {
		return parts[i + 1];
	};

	for (int i = 0; i < parts.size(); i++)
	{
		if (parts.at(i) == ID_HEADER)
		{
			tmp_value = getValue(i);
			macro_item.id = tmp_value;
			continue;
		}
		if (parts.at(i) == MS_HEADER)
		{
			tmp_value = getValue(i);
			macro_item.ms = stoi(tmp_value);
			continue;
		}
		if (parts.at(i) == CP_HEADER)
		{
			tmp_value = getValue(i);
			std::vector<std::string> sprt;
			{
				std::stringstream stream(tmp_value);
				std::string tmp_i;

				while (std::getline(stream, tmp_i, ','))
				{
					sprt.push_back(tmp_i);
				}
			}
			{
				Point cp;
				try
				{
					cp.x = stoi(sprt.at(0));
					cp.y = stoi(sprt.at(1));
				}
				catch (const std::exception&)
				{
					message::logMessage("MACRO IMPORT ERROR", "CLICK POINT DATA IS NOT IN CORRECT FORM.",red);
					message::logMessage("INFO", "IMPORT PROCESS ABORTED.", yellow);
					aborted = true;
					break;
				}
				macro_item.click_point = cp;
			}
			continue;
		}
		if (parts.at(i) == SSCAN_AREA_HEADER)
		{
			tmp_value = getValue(i);
			std::vector<std::string> sprt;
			{
				std::stringstream stream(tmp_value);
				std::string tmp_i;

				while (std::getline(stream, tmp_i, ','))
				{
					sprt.push_back(tmp_i);
				}
			}
			{
				Rectangle rect;
				try
				{
					rect.x = stoi(sprt.at(0));
					rect.y = stoi(sprt.at(1));
					rect.width = stoi(sprt.at(2));
					rect.height = stoi(sprt.at(3));
				}
				catch (const std::exception&)
				{
					message::logMessage("MACRO IMPORT ERROR", "RECTANGLE DATA IS NOT IN CORRECT FORM.", red);
					message::logMessage("INFO", "IMPORT PROCESS ABORTED.", yellow);
					aborted = true;
					break;
				}
				macro_item.screen_scan_area = rect;
			}
			continue;
		}
		if (parts.at(i) == KEY_HEADER)
		{
			tmp_value = getValue(i);
			macro_item.key = tmp_value;
			continue;
		}

	}
	if (!aborted)
	{
		global_macros.push_back(macro_item);
		message::logMessage("SUCCESS","MACRO IMPORT PROCESS SUCCESSFULLY COMPLETED.",green);
	}
}



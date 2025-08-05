#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include <functional>
#include <sstream>
#include "macro.h";

#define ID_HEADER "ID"
#define MS_HEADER "ID"
#define CP_HEADER "ID"
#define SSCAN_AREA_HEADER "ID"
#define KEY_HEADER "ID"

struct Point {
public:
	int x;
	int y;
};

struct Rectangle {
public:
	int x1, x2;
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

void import_macro(std::vector<std::string> parts) // MACRO ITEM RECEIVE SAMPLE : ID:macroHeader:MS:macroSpeed:CLICK_POINT:clickPosition.x,clickPosition.y:SCAN_RECT:scanArea.x,scanArea.y,scanArea.width,scanArea.height:HOTKEY:macroHotkey
{
	m_item macro_item;
	std::string tmp_value;
	std::function<std::string(int)> getValue = [&parts](int current_index) {
		std::string result;
		result = parts.at(current_index + 1);
		return result;
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
			Point cp;
			try
			{
				cp.x = stoi(sprt.at(0));
				cp.y = stoi(sprt.at(1));
			}
			catch (const std::exception&)
			{
				
			}
			cp.x = stoi(sprt.at(0));
			cp.y = stoi(sprt.at(1));
			macro_item.click_point = cp;
			continue;
		}

	}
}



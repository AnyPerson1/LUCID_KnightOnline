#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <optional>

class macro {
public:
	void import_macro(std::vector<std::string>);
public:
	void activate_macro(std::string id);
public:
	void deactivate_macro(std::string id);
public:
	static std::vector<class m_item> global_macros;
};

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
	bool active = false;
};


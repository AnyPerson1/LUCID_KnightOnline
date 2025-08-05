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
};


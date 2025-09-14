#include "parser.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

std::vector<std::string> split_string(const std::string &string, const char delim) {
	std::vector<std::string> sections;
	std::istringstream iss(string);
	std::string section;
	while (!iss.eof()) {
		std::getline(iss, section, delim);
		sections.push_back(section);
	}
	return sections;
}
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <boost/json.hpp>
// #include <iostream>

/// @brief trim string ref driectly in place of set of charachter 
/// @param str string to trim
/// @param arr set of char to trim
void strTrim(std::string& str, std::string arr = " \t\r\n") {
	size_t start = str.find_first_not_of(arr);
	if (start > str.length()) {
		str = "";
		return;
	}
	size_t end = str.find_last_not_of(arr);
	str = str.substr(start, end - start + 1);	
}

/// @brief open and load file in a string
/// @param filePath path absolute or relative to file to open
/// @return stingify file
std::string fileToStr(const std::string& filePath) {
    std::ifstream file(filePath);
	if (!file.is_open())
		throw std::runtime_error("Could not open File: " + filePath);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}


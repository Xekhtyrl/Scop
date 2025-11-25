#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
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
std::string fileToStr(std::string filePath) {
	std::string fileStr;
	std::string tmp;
	std::ifstream file;
	
	file.open(filePath.c_str(), std::ios::in);
	if (!file)
		throw std::runtime_error("Could not open File: " + filePath);
	while(getline(file, tmp))
		fileStr.append(tmp + "\n");
	file.close();
	strTrim(fileStr);
	return fileStr;
}


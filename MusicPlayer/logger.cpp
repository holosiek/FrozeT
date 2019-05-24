#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "logger.h"

void Logger::log(const std::string a_text){
	Logger::log(std::wstring(a_text.begin(), a_text.end()));
}
void Logger::log(const std::string a_prefix, const std::string a_text){
	Logger::log(L"[" + std::wstring(a_prefix.begin(), a_prefix.end()) + L"] " + std::wstring(a_text.begin(), a_text.end()));
}
void Logger::log(const char* a_text){
	std::string convText = std::string(a_text);
	Logger::log(std::wstring(convText.begin(), convText.end()));
}
void Logger::log(const char* a_prefix, const char* a_text){
	std::string convPrefix = std::string(a_prefix);
	std::string convText = std::string(a_text);
	Logger::log(L"[" + std::wstring(convPrefix.begin(), convPrefix.end()) + L"] " + std::wstring(convText.begin(), convText.end()));
}
void Logger::log(const std::wstring a_text){
	std::wofstream file("log.txt", std::ios_base::app);
	time_t timeNow = time(nullptr);
	char buffer[80];
	strftime(buffer, 80, "[%F %R] ", localtime(&timeNow));

	file << buffer << a_text << "\n";
	file.close();
}
void Logger::log(const std::wstring a_prefix, const std::wstring a_text){
	Logger::log(L"[" + a_prefix + L"] " + a_text);
}

Logger::Logger(){
	std::wofstream file("log.txt");
	file.close();
}


Logger logSys;
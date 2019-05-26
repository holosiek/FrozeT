#ifndef LOGGER
#define LOGGER

class Logger{
	public:
		Logger();
		void log(const std::wstring a_prefix, const std::wstring a_text);
		void log(const std::wstring a_text);
		void log(const std::string a_prefix, const std::string a_text);
		void log(const std::string a_text);
		void log(const char* a_prefix, const char* a_text);
		void log(const char* a_text);
};

#endif LOGGER
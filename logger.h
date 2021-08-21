#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <string>
#include <fstream>
#include <memory>
#define LOG_FROM __FILE__, __LINE__

class Logger
{
		std::shared_ptr<std::ofstream> logStream;
		static std::string timestamp();
	public:
		Logger()=default;
		Logger(const Logger& other)=default;
		Logger(const std::string& logFileName);
		~Logger()=default;
		bool openFile(const std::string& logFileName);
		void write(const std::string& msg,const std::string& from="");
		void write(const std::string& msg,const std::string& from_file,int from_line);
};

extern Logger logger;

#endif

#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <QString>

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

		Logger(const QString& logFileName);
		~Logger()=default;

		bool openFile(const QString& logFileName);
		void write(const QString& msg, const QString& from="");
		void write(const QString& msg, const QString& from_file, int from_line);
};

extern Logger logger;

#endif

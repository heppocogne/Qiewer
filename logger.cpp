#include "logger.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>


Logger::Logger(const std::string& logFileName)
	:logStream(new std::ofstream(logFileName,std::ios_base::trunc))
{

}


std::string Logger::timestamp()
{
	auto now = std::chrono::system_clock::now();
	auto now_c = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(localtime(&now_c), "%Y/%m/%d %H:%M:%S");

	return ss.str();
}


bool Logger::openFile(const std::string& logFileName)
{
	if(logStream!=nullptr&&logStream->is_open()) {
		logStream->close();
	}
	logStream=std::shared_ptr<std::ofstream>(new std::ofstream(logFileName,std::ios_base::trunc));
	return logStream->good();
}


void Logger::write(const std::string& msg,const std::string& from_file,int from_line)
{
	write(msg,from_file+":"+std::to_string(from_line));
}


void Logger::write(const std::string& msg,const std::string& from)
{
	(*logStream)<<'['<<timestamp()<<']';
	if(from!="") {
		(*logStream)<<'['<<from<<']';
	}
	(*logStream)<<msg<<std::endl;
}

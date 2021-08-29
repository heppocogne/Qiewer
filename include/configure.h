#ifndef CONFIGURE_H_INCLUDED
#define CONFIGURE_H_INCLUDED

#include <QString>

#include "logger.h"

#include <fstream>

#define LATEST_CONFIGURE_FORMAT 3


template<int ver>
struct LaunchConfigure_impl {
	//dummy; write default values here
	constexpr static const int width=1600;
	constexpr static const int height=900;
	constexpr static const bool maximized=false;
	constexpr static const int dropFilesLimit=8;

	//when rememberLastDirectory=true, 'directory' is the last directory, otherwise, user-set directory
	#define LaunchConfigure_default_directory ""
	constexpr static const char* directory=LaunchConfigure_default_directory;
	constexpr static const bool rememberLastDirectory=true;
};
using LaunchConfigure_default=LaunchConfigure_impl<-1>;

//latest version here
template<>
struct LaunchConfigure_impl<LATEST_CONFIGURE_FORMAT> {
	int width=LaunchConfigure_default::width,
	    height=LaunchConfigure_default::height;
	bool maximized=LaunchConfigure_default::maximized;
	int dropFilesLimit=LaunchConfigure_default::dropFilesLimit;
	char directory[512]=LaunchConfigure_default_directory;	//LaunchConfigure_default::directory is not available
	bool rememberLastDirectory=LaunchConfigure_default::rememberLastDirectory;
};

class ConfigureIO
{
		//std::string configureFileName;
		QString configureFileName;
	public:
		//latest version
		constexpr static const int latestFormat=LATEST_CONFIGURE_FORMAT;
		ConfigureIO();
		ConfigureIO(const QString& _configureFileName);
		ConfigureIO(const ConfigureIO& other)=delete;
		bool load(const QString& _configureFileName);
		bool load(void);
		bool save(void)const;
		void reset(void);

		using LaunchConfigure=LaunchConfigure_impl<latestFormat>;
		LaunchConfigure config;
};

extern ConfigureIO configureIO;

#endif

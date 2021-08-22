#ifndef CONFIGURE_H_INCLUDED
#define CONFIGURE_H_INCLUDED

#include "logger.h"
#include <fstream>

#define LATEST_CONFIGURE_FORMAT 2


template<int ver>
struct LaunchConfigure_impl {
	//dummy; write default values here
	constexpr static const int width=1600;
	constexpr static const int height=900;
	constexpr static const bool maximized=false;
	constexpr static const int dropFilesLimit=8;
};
using LaunchConfigure_default=LaunchConfigure_impl<-1>;

//latest version here
template<>
struct LaunchConfigure_impl<LATEST_CONFIGURE_FORMAT> {
	int width=LaunchConfigure_default::width,
	    height=LaunchConfigure_default::height;
	bool maximized=LaunchConfigure_default::maximized;
	int dropFilesLimit=LaunchConfigure_default::dropFilesLimit;
};

class ConfigureIO
{
		const std::string configureFileName;
		template<int ver>
		bool loadOldFormat(void);
	public:
		//latest version
		constexpr static const int latestFormat=LATEST_CONFIGURE_FORMAT;
		ConfigureIO(const std::string& _configureFileName);
		~ConfigureIO();
		bool load(void);
		bool save(void);
		void reset(void);

		using LaunchConfigure=LaunchConfigure_impl<latestFormat>;
		LaunchConfigure config;
};


#endif

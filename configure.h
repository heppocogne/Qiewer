#ifndef CONFIGURE_H_INCLUDED
#define CONFIGURE_H_INCLUDED

#include "logger.h"
#include <fstream>


template<int ver>
struct LaunchConfigure_impl {	
	//dummy
};

//latest version here
template<>
struct LaunchConfigure_impl<1> {
	int width=1200,
	    height=675;
	bool maximized=false;
};

class ConfigureIO
{
		const std::string configureFileName;
		template<int ver>
		bool loadOldFormat(void);
	public:
		//latest version
		constexpr static const int latestFormat=1;
		ConfigureIO(const std::string& _configureFileName);
		~ConfigureIO();
		bool load(void);
		bool save(void);

		using LaunchConfigure=LaunchConfigure_impl<latestFormat>;
		LaunchConfigure config;
};

#endif

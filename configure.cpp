#include "configure.h"

//old formats here
template<>
struct LaunchConfigure_impl<0> {
	int width=LaunchConfigure_default::width,
	    height=LaunchConfigure_default::height;
};

template<>
struct LaunchConfigure_impl<1> {
	int width=LaunchConfigure_default::width,
	    height=LaunchConfigure_default::height;
	bool maximized=LaunchConfigure_default::maximized;
};


ConfigureIO::ConfigureIO(const std::string& _configureFileName)
	:configureFileName(_configureFileName)
{

}

ConfigureIO::~ConfigureIO()
{
	save();
}


bool ConfigureIO::load(void)
{
	std::fstream configStream(configureFileName, std::ios_base::binary|std::ios_base::in);

	bool successful=true;

	if(configStream) {
		logger.write("load config file:	"+configureFileName, LOG_FROM);

		int format;
		configStream.read(reinterpret_cast<char*>(&format), sizeof(int));

		switch(format) {
			case 0:
				configStream.read(reinterpret_cast<char*>(&config), sizeof(LaunchConfigure_impl<0>));
				break;

			case 1:
				configStream.read(reinterpret_cast<char*>(&config), sizeof(LaunchConfigure_impl<1>));
				break;

			case latestFormat:
				configStream.read(reinterpret_cast<char*>(&config), sizeof(ConfigureIO::LaunchConfigure));
				break;

			default :
				successful=false;
		}
	} else {
		successful=false;
	}

	configStream.close();
	return successful;
}


bool ConfigureIO::save(void)
{
	std::fstream configStream(configureFileName, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);

	if(configStream) {
		logger.write("save config file:	"+configureFileName, LOG_FROM);

		const int format=latestFormat;
		configStream.write(reinterpret_cast<const char*>(&format), sizeof(int));
		configStream.write(reinterpret_cast<const char*>(&config), sizeof(ConfigureIO::LaunchConfigure));
		configStream.close();

		return true;
	} else {
		return false;
	}
}


void ConfigureIO::reset(void)
{
	config=LaunchConfigure();
}

#include "version.h"

//Version version(0, 0, 0, "test");

Version::Version(const int _major, const int _minor, const int _patch, const std::string& _identifier)
	: major(_major),
	  minor(_minor),
	  patch(_patch),
	  identifier(_identifier)
{
}

Version::operator std::string() const
{
	std::string base = std::to_string(major) + '.' + std::to_string(minor) + '.' + std::to_string(patch);
	if(identifier == "") {
		return base;
	} else {
		return base + '-' + identifier;
	}
}

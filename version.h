#ifndef VERSION_H_INCLUDED
#define VERSION_H_INCLUDED

#include <string>

struct Version {
	const int major, minor, patch;
	const std::string identifier;

	Version(const int _major, const int _minor, const int _patch, const std::string& _identifier = "");

	operator std::string() const;
};

extern const Version version;

#endif

#ifndef VERSION_H_INCLUDED
#define VERSION_H_INCLUDED

#include <string>
#include <QString>

struct Version {
	const int major, minor, patch;
	const QString identifier;

	Version(const int _major, const int _minor, const int _patch, const QString& _identifier = "");

	operator QString()const;
};

extern const Version version;

#endif

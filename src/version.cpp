#include "version.h"

Version::Version(const int _major, const int _minor, const int _patch, const QString& _identifier)
	: major(_major),
	  minor(_minor),
	  patch(_patch),
	  identifier(_identifier){}

Version::operator QString() const{
	QString base=QString::number(major)+'.'+QString::number(minor)+ '.' +QString::number(patch);
	if(identifier == "") {
		return base;
	} else {
		return base + '-' + identifier;
	}
}

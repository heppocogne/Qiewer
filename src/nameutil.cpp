#include "nameutil.h"


QString extractDirectoryName(const QString& fileName){
	int idx=fileName.length()-1;
	for(; 0<=idx; idx--) {
		if(fileName[idx]=='\\'||fileName[idx]=='/') {
			break;
		}
	}
	//idx=-1 or fileName[idx]=\ or /
	return fileName.mid(0, idx);
}


QString extractFileName(const QString& fileName)
{
	int idx=fileName.length()-1;
	for(; 0<=idx; idx--) {
		if(fileName[idx]=='\\'||fileName[idx]=='/') {
			break;
		}
	}
	//idx=-1 or fileName[idx]=\ or /
	return fileName.mid(idx+1);
}
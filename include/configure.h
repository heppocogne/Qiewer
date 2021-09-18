#ifndef CONFIGURE_H_INCLUDED
#define CONFIGURE_H_INCLUDED


#include <QString>
#include <QWidget>

#include "logger.h"

class Configure
{
		QString configureFileName;
		Configure(const Configure& other)=default;
	public:
		Configure();
		bool load(const QString& configureFileName);
		bool save(void);
		void reset(void);
		
		void openConfigureDialog(void);
		bool openCloseConfirmDialog(void);

		int windowWidth;
		int windowHeight;
		bool maximized;
		QString directory;
		bool rememberLastDirectory;
		bool allowDuplicatedFiles;
		bool confirmBeforeQuit;
};
extern Configure configure;


#endif

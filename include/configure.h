#ifndef CONFIGURE_H_INCLUDED
#define CONFIGURE_H_INCLUDED


#include <QString>

#include "logger.h"


class Configure
{
		QString configureFileName;
		Configure(const Configure& other)=default;
		
		template<typename... types>
		static void connect(types... args);
	public:
		Configure();
		bool load(const QString& configureFileName);
		bool save(void)const;
		void reset(void);
		
		void openConfigureDialog(void);
		bool openCloseConfirmDialog(void);


		int windowWidth;
		int windowHeight;
		bool maximized;
		enum WindowSizeMode{REMEMBER_SIZE, ALWAYS_MAXIMIZED, USE_DEFALUT_SIZE};
		int windowSizeMode;
		
		QString directory;
		bool rememberLastDirectory;
		
		double virtualScaleMax;
		double virtualScaleMin;
		double zoomManipulationPrecision;
		bool raster_antialiasing;
		bool svg_scalingUnlimited;
		
		bool allowDuplicatedFiles;
		bool confirmBeforeQuit;
};
extern Configure configure;


#endif

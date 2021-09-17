#include "configure.h"

#include <Qt>
#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>

const Configure defaultValues;


Configure::Configure()
	:windowWidth(1600),
	 windowHeight(900),
	 maximized(false),
	 directory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)),
	 rememberLastDirectory(true),
	 allowDuplicatedFiles(false),
	 confirmBeforeQuit(true)
{

}

bool Configure::load(const QString& _configureFileName)
{
	this->configureFileName=_configureFileName;
	QFile configureFile(configureFileName);
	if(configureFile.open(QFile::ReadOnly | QFile::Text)) {
		QTextStream ifs(&configureFile);

		QJsonDocument doc=QJsonDocument::fromJson(ifs.readAll().toUtf8());
		QJsonObject obj=doc.object();

#define obj_Read(id) if(obj.contains(#id)) id=obj[#id]
		obj_Read(windowWidth).toInt();
		obj_Read(windowHeight).toInt();
		obj_Read(maximized).toBool();
		obj_Read(directory).toString();
		obj_Read(rememberLastDirectory).toBool();
		obj_Read(allowDuplicatedFiles).toBool();
		obj_Read(confirmBeforeQuit).toBool();
#undef obj_Read
		configureFile.close();
		return true;
	} else {
		return false;
	}
}


bool Configure::save(void)
{
	QFile configureFile(configureFileName);
	if(configureFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		QTextStream ofs(&configureFile);

		QJsonObject obj;
#define obj_Write(id) obj[#id]=id
		obj_Write(windowWidth);
		obj_Write(windowHeight);
		obj_Write(maximized);
		obj_Write(directory);
		obj_Write(rememberLastDirectory);
		obj_Write(allowDuplicatedFiles);
		obj_Write(confirmBeforeQuit);
#undef obj_Write
		QJsonDocument doc=QJsonDocument(obj);
		ofs<<doc.toJson();
		configureFile.close();
		return true;
	} else {
		return false;
	}
}


void Configure::reset(void)
{
	*this=Configure();
}


void Configure::openConfigureDialog(QWidget* dialogParent)
{
	QDialog* configureDialog=new QDialog(dialogParent);
	configureDialog->setWindowTitle("Setting");

	QBoxLayout* const mainLayout=new QVBoxLayout(configureDialog);
	QCheckBox* const rememberWindowSize=new QCheckBox("Remember last window size", configureDialog);

	//maynLayout->

	if(dialogParent==nullptr) {
		delete configureDialog;
	}
}


bool Configure::openCloseConfirmDialog(QWidget* dialogParent)
{
	QDialog* confirmDialog=new QDialog(dialogParent);
	confirmDialog->setWindowTitle("Close Confirmation");

	QBoxLayout* const mainLayout=new QVBoxLayout(confirmDialog);
	QBoxLayout* const subLayout=new QHBoxLayout(confirmDialog);

	QLabel* const questionMessage=new QLabel("Exit?", confirmDialog);
	QPushButton* const yesButton=new QPushButton("Yes", confirmDialog);
	QPushButton* const noButton=new QPushButton("No", confirmDialog);
	QCheckBox* const neverAskAgain=new QCheckBox("Don't ask me again", confirmDialog);

	subLayout->addWidget(yesButton);
	subLayout->addWidget(noButton);
	mainLayout->addWidget(questionMessage, 0, Qt::AlignHCenter);
	mainLayout->addLayout(subLayout);
	mainLayout->addWidget(neverAskAgain, 0, Qt::AlignHCenter);

	confirmDialog->setLayout(mainLayout);

	bool result=false;
	int checkState=configure.confirmBeforeQuit ? Qt::Unchecked : Qt::Checked;
	neverAskAgain->setCheckState(static_cast<Qt::CheckState>(checkState));

	QObject::connect(yesButton, &QPushButton::clicked, [&] {
		result=true;
		confirmDialog->close();
	});
	QObject::connect(noButton, &QPushButton::clicked, [&] {
		result=false;
		confirmDialog->close();
	});
	QObject::connect(neverAskAgain, &QCheckBox::stateChanged, [&](int state) {
		checkState=state;
	});

	confirmDialog->exec();

	configure.confirmBeforeQuit=(checkState==Qt::Unchecked);

	if(dialogParent==nullptr) {
		delete confirmDialog;
	}

	return result;
}



#if 0
//older formats here
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

template<>
struct LaunchConfigure_impl<2> {
	int width=LaunchConfigure_default::width,
	    height=LaunchConfigure_default::height;
	bool maximized=LaunchConfigure_default::maximized;
	int dropFilesLimit=LaunchConfigure_default::dropFilesLimit;
};

template<>
struct LaunchConfigure_impl<3> {
	int width=LaunchConfigure_default::width,
	    height=LaunchConfigure_default::height;
	bool maximized=LaunchConfigure_default::maximized;
	int dropFilesLimit=LaunchConfigure_default::dropFilesLimit;
	char directory[512]=LaunchConfigure_default_directory;	//LaunchConfigure_default::directory is not available
	bool rememberLastDirectory=LaunchConfigure_default::rememberLastDirectory;
};


ConfigureIO::ConfigureIO()
	:configureFileName("")
{

}

ConfigureIO::ConfigureIO(const QString& _configureFileName)
	:configureFileName(_configureFileName)
{

}


bool ConfigureIO::load(void)
{
	std::fstream configStream(configureFileName.toStdString(), std::ios_base::binary|std::ios_base::in);

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

			case 2:
				configStream.read(reinterpret_cast<char*>(&config), sizeof(LaunchConfigure_impl<2>));
				break;

			case 3:
				configStream.read(reinterpret_cast<char*>(&config), sizeof(LaunchConfigure_impl<3>));
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


bool ConfigureIO::load(const QString& _configureFileName)
{
	if(configureFileName=="") {
		configureFileName=_configureFileName;
		return load();
	} else {
		return false;
	}
}


bool ConfigureIO::save(void)const
{
	std::fstream configStream(configureFileName.toStdString(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);

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


void ConfigureIO::openConfigureDialog(QWidget* dialogParent)
{
	QDialog* configureDialog=new QDialog(dialogParent);
	configureDialog->setWindowTitle("Setting");

	QBoxLayout* const mainLayout=new QVBoxLayout(configureDialog);
	QCheckBox* const rememberWindowSize=new QCheckBox("Remember last window size", configureDialog);

	//maynLayout->

	if(dialogParent==nullptr) {
		delete configureDialog;
	}
}



bool ConfigureIO::openCloseConfirmDialog(QWidget* dialogParent)
{
	QDialog* confirmDialog=new QDialog(dialogParent);
	confirmDialog->setWindowTitle("Close Confirmation");

	QBoxLayout* const mainLayout=new QVBoxLayout(confirmDialog);
	QBoxLayout* const subLayout=new QHBoxLayout(confirmDialog);

	QLabel* const questionMessage=new QLabel("Exit?", confirmDialog);
	QPushButton* const yesButton=new QPushButton("Yes", confirmDialog);
	QPushButton* const noButton=new QPushButton("No", confirmDialog);
	QCheckBox* const neverAskAgain=new QCheckBox("Don't ask me again", confirmDialog);

	subLayout->addWidget(yesButton);
	subLayout->addWidget(noButton);
	mainLayout->addWidget(questionMessage, 0, Qt::AlignHCenter);
	mainLayout->addLayout(subLayout);
	mainLayout->addWidget(neverAskAgain, 0, Qt::AlignHCenter);

	confirmDialog->setLayout(mainLayout);

	bool result=false;
	int checkState=configureIO.config.confirmBeforeQuit ? Qt::Unchecked : Qt::Checked;
	neverAskAgain->setCheckState(static_cast<Qt::CheckState>(checkState));

	QObject::connect(yesButton, &QPushButton::clicked, [&] {
		result=true;
		confirmDialog->close();
	});
	QObject::connect(noButton, &QPushButton::clicked, [&] {
		result=false;
		confirmDialog->close();
	});
	QObject::connect(neverAskAgain, &QCheckBox::stateChanged, [&](int state) {
		checkState=state;
	});

	confirmDialog->exec();

	configureIO.config.confirmBeforeQuit=(checkState==Qt::Unchecked);

	if(dialogParent==nullptr) {
		delete confirmDialog;
	}

	return result;
}
#endif

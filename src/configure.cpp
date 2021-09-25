#include "configure.h"

#include <Qt>
#include <QFile>
#include <QObject>
#include <QDialog>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>
#include <QScreen>
#include <QGuiApplication>
#include "ui_close_confirmation.h"
#include "ui_configure.h"


template<typename... types>
void Configure::connect(types... args)
{
	QObject::connect(args...);
}


Configure::Configure()
	:windowWidth(1600),
	 windowHeight(900),
	 maximized(false),
	 windowSizeMode(REMEMBER_SIZE),
	 directory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)),
	 rememberLastDirectory(true),
	 virtualScaleMax(10.0),
	 virtualScaleMin(0.1),
	 zoomManipulationPrecision(1.0),
	 raster_antialiasing(true),
	 svg_scalingUnlimited(false),
	 allowDuplicatedFiles(false),
	 confirmBeforeQuit(true)
{

}

bool Configure::load(const QString& _configureFileName)
{
	reset();
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
		obj_Read(windowSizeMode).toInt();
		obj_Read(directory).toString();
		obj_Read(rememberLastDirectory).toBool();
		obj_Read(allowDuplicatedFiles).toBool();
		obj_Read(confirmBeforeQuit).toBool();
		obj_Read(virtualScaleMax).toDouble();
		obj_Read(virtualScaleMin).toDouble();
		obj_Read(zoomManipulationPrecision).toDouble();
		obj_Read(raster_antialiasing).toBool();
		obj_Read(svg_scalingUnlimited).toBool();
#undef obj_Read
		configureFile.close();
		return true;
	} else {
		return false;
	}
}


bool Configure::save(void)const
{
	QFile configureFile(configureFileName);
	if(configureFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		QTextStream ofs(&configureFile);

		QJsonObject obj;
#define obj_Write(id) obj[#id]=id
		obj_Write(windowWidth);
		obj_Write(windowHeight);
		obj_Write(maximized);
		obj_Write(windowSizeMode);
		obj_Write(directory);
		obj_Write(rememberLastDirectory);
		obj_Write(allowDuplicatedFiles);
		obj_Write(confirmBeforeQuit);
		obj_Write(virtualScaleMax);
		obj_Write(virtualScaleMin);
		obj_Write(zoomManipulationPrecision);
		obj_Write(raster_antialiasing);
		obj_Write(svg_scalingUnlimited);
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


void Configure::openConfigureDialog(void)
{
	QDialog* const configureDialog=new QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	Ui::ConfigureDialog ui;
	ui.setupUi(configureDialog);

	Configure backup=*this;

	ui.


	configureDialog->setFixedSize(configureDialog->size());
	configureDialog->exec();


	delete configureDialog;
}


bool Configure::openCloseConfirmDialog(void)
{
	QDialog* const confirmDialog=new QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	Ui::ConfirmDialog ui;
	ui.setupUi(confirmDialog);

	bool result=false;
	int checkState=configure.confirmBeforeQuit ? Qt::Unchecked : Qt::Checked;
	ui.neverAskAgain->setCheckState(static_cast<Qt::CheckState>(checkState));

	connect(ui.okButton, &QPushButton::clicked, [&] {result=true;});
	connect(ui.cancelButton, &QPushButton::clicked, [&] {result=false;});
	connect(ui.neverAskAgain, &QCheckBox::stateChanged, [&](int state) {
		checkState=state;
	});

	confirmDialog->setFixedSize(confirmDialog->size());

	const Configure backup=*this;
	confirmDialog->exec();

	configure.confirmBeforeQuit=(checkState==Qt::Unchecked);
	if(!result)
		*this=backup;

	delete confirmDialog;
	return result;
}

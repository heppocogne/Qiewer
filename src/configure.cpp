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
#include <QSize>
#include <QGuiApplication>
#include <QFileDialog>
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

	Configure this_copy=*this;

	//window size
	const auto& available=QGuiApplication::primaryScreen()->availableSize();
	ui.widthEdit->setMaximum(available.width());
	ui.heightEdit->setMaximum(available.height());
	connect(ui.rememberSize, 		&QRadioButton::clicked, [&] {this_copy.windowSizeMode=REMEMBER_SIZE;});
	connect(ui.alwaysMaximized, 	&QRadioButton::clicked, [&] {this_copy.windowSizeMode=ALWAYS_MAXIMIZED;});
	connect(ui.useDefaultSize, 		&QRadioButton::clicked, [&] {this_copy.windowSizeMode=USE_DEFALUT_SIZE;});
	connect(ui.widthEdit,	QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
		this_copy.windowWidth=value;
	});
	connect(ui.heightEdit,	QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
		this_copy.windowHeight=value;
	});
	ui.widthEdit->setValue(windowWidth);
	ui.heightEdit->setValue(windowHeight);
	switch(windowSizeMode) {
		case REMEMBER_SIZE:
			ui.rememberSize->setChecked(true);
			break;
		case ALWAYS_MAXIMIZED:
			ui.alwaysMaximized->setChecked(true);
			break;
		case USE_DEFALUT_SIZE:
			ui.useDefaultSize->setChecked(true);
			break;
	}

	//file selector
	connect(ui.rememberDirectory,	QRadioButton::clicked, [&] {this_copy.rememberLastDirectory=true;});
	connect(ui.useDefaultDirectory,	QRadioButton::clicked, [&] {this_copy.rememberLastDirectory=false;});
	connect(ui.directoryPathEdit, &QLineEdit::textEdited, [&](const QString& text) {
		this_copy.directory=text;
	});
	connect(ui.browseButton, &QPushButton::clicked, [&] {
		const QString& selected=QFileDialog::getExistingDirectory(configureDialog, "Select default directory", this_copy.directory);
		ui.directoryPathEdit->setText(selected);
	});
	if(rememberLastDirectory)
		ui.rememberDirectory->setChecked(true);
	else
		ui.useDefaultDirectory->setChecked(true);
	ui.directoryPathEdit->setText(directory);

	//zoom manipulation
	connect(ui.minEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double value) {
		this_copy.virtualScaleMin=value;
	});
	connect(ui.maxEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double value) {
		this_copy.virtualScaleMax=value;
	});
	connect(ui.precisionEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double value) {
		this_copy.zoomManipulationPrecision=value;
	});
	connect(ui.enableAntialiasing, &QCheckBox::toggled, [&](bool value) {
		this_copy.raster_antialiasing=value;
	});
	connect(ui.unlimitSvgScaling, &QCheckBox::toggled, [&](bool value) {
		this_copy.svg_scalingUnlimited=value;
	});
	ui.minEdit->setValue(virtualScaleMin);
	ui.maxEdit->setValue(virtualScaleMax);
	ui.precisionEdit->setValue(zoomManipulationPrecision);
	ui.enableAntialiasing->setChecked(raster_antialiasing);
	ui.unlimitSvgScaling->setChecked(svg_scalingUnlimited);

	//other
	connect(ui.closeConfirmation, &QCheckBox::toggled, [&](bool value) {
		this_copy.confirmBeforeQuit=value;
	});
	connect(ui.duplicatedImages, &QCheckBox::toggled, [&](bool value) {
		this_copy.allowDuplicatedFiles=value;
	});
	ui.closeConfirmation->setChecked(confirmBeforeQuit);
	ui.duplicatedImages->setChecked(allowDuplicatedFiles);

	//buttons
	const auto& commitChange=[&] {
		*this=this_copy;
	};
	connect(ui.doneButton, &QPushButton::clicked, commitChange);
	connect(ui.applyButton, &QPushButton::clicked, commitChange);


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

	Configure this_copy=*this;
	confirmDialog->setFixedSize(confirmDialog->size());

	confirmDialog->exec();

	this_copy.confirmBeforeQuit=(checkState==Qt::Unchecked);
	if(result)
		*this=this_copy;

	delete confirmDialog;
	return result;
}

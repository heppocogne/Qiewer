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
#include <QMessageBox>

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
	QDialog* configureDialog=new QDialog(dialogParent, (Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint) & ~Qt::WindowContextHelpButtonHint);
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
	QDialog* confirmDialog=new QDialog(dialogParent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	confirmDialog->setWindowTitle("Close Confirmation");

	QBoxLayout* const mainLayout=new QVBoxLayout(confirmDialog);
	mainLayout->setContentsMargins(80, 40, 80, 40);
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
	confirmDialog->setFixedSize(confirmDialog->sizeHint());

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

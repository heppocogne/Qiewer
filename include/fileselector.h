#ifndef FILESELECTOR_H_INCLUDED
#define FILESELECTOR_H_INCLUDED

#include <QObject>
#include <QString>
#include <QWidget>

#include "configure.h"

class FileSelector: public QObject
{
		Q_OBJECT
	public:
		FileSelector(QWidget* parent=nullptr);

	public slots:
		void open(void);

	signals:
		void fileSelected(QString filename);
};

#endif

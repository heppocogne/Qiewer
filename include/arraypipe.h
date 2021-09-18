#ifndef ARRAYPIPE_H_INCLUDED
#define ARRAYPIPE_H_INCLUDED

#include <QSharedMemory>
#include <QString>
#include <QStringList>

#include <chrono>
#include <vector>

class ArrayPipe
{
		const QString keyPrefix;
		QSharedMemory* const counter;
		//holds shared memory objects to prevent them from being released
		std::vector<QSharedMemory*> elements;

		const std::chrono::milliseconds timeout;
		const std::chrono::milliseconds retry;

		char buf[1024];

		bool waitForLock(QSharedMemory* m);
		void clearElements(void);
		ArrayPipe()=delete;
		ArrayPipe(const ArrayPipe& other)=delete;
	public:
		ArrayPipe(const QString& _keyPrefix=QString(""), int timeout_ms=5000, int retry_ms=100);
		~ArrayPipe();
		bool attach(void);
		bool create(void);
		bool paste(const QStringList& values);
		QStringList cut(void);
		int currentSize(void);

		constexpr static const int bufferSize=sizeof(buf);
};

#endif

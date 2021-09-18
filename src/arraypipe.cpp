#include "arraypipe.h"

#include <cstring>
#include <thread>


ArrayPipe::ArrayPipe(const QString& _keyPrefix, int timeout_ms, int retry_ms)
	:keyPrefix(_keyPrefix),
	 counter(new QSharedMemory(keyPrefix+"count")),
	 timeout(std::chrono::milliseconds(timeout_ms)),
	 retry(std::chrono::milliseconds(retry_ms))
{
	
}


bool ArrayPipe::attach(void)
{
	return counter->attach();
}

bool ArrayPipe::create(void)
{
	if(counter->create(sizeof(int))) {
		const int count=0;
		counter->lock();
		memcpy(counter->data(), &count, sizeof(int));
		counter->unlock();
		return true;
	}
	return false;
}


ArrayPipe::~ArrayPipe()
{
	delete counter;
}


bool ArrayPipe::waitForLock(QSharedMemory* m)
{
	for(auto t=std::chrono::milliseconds(0); t<timeout; t+=retry) {
		if(m->lock()) {
			return true;
		}
		std::this_thread::sleep_for(retry);
	}
	return false;
}


void ArrayPipe::clearElements(void)
{
	for(QSharedMemory* elem: elements)
		delete elem;

	elements.clear();
}


bool ArrayPipe::paste(const QStringList& values)
{
	if(waitForLock(counter)) {
		const int count=values.size();
		memcpy(counter->data(), &count, sizeof(int));

		clearElements();

		for(int i=0; i<values.size(); i++) {
			const auto local8bit=values[i].toLocal8Bit();

			QSharedMemory* const elem=new QSharedMemory(keyPrefix+QString::number(i), counter);
			if(!elem->attach())
				elem->create(bufferSize);
			elem->lock();
			strcpy(reinterpret_cast<char*>(elem->data()), (local8bit.size()<bufferSize)?local8bit.constData():"");
			elem->unlock();
			elements.push_back(elem);
		}

		counter->unlock();
		return true;
	}
	return false;
}


QStringList ArrayPipe::cut(void)
{
	QStringList values;
	if(waitForLock(counter)) {
		int count;
		memcpy(reinterpret_cast<char*>(&count), counter->constData(), sizeof(int));

		if(count!=0) {
			for(int i=0; i<count; i++) {
				QSharedMemory elem(keyPrefix+QString::number(i), counter);
				elem.attach(QSharedMemory::ReadOnly);
				elem.lock();
				strcpy(buf, reinterpret_cast<const char*>(elem.constData()));
				elem.unlock();
				values<<const_cast<const char*>(buf);
			}

			count=0;
			memcpy(counter->data(), &count, sizeof(int));
		}

		counter->unlock();
		return values;
	}
	return values;	//empty
}


int ArrayPipe::currentSize(void)
{
	if(waitForLock(counter)) {
		int count;
		memcpy(reinterpret_cast<char*>(&count), counter->constData(), sizeof(int));
		counter->unlock();
		return count;
	}
	return -1;
}

#include "ConcurrencyVector.h"

template <class T, class Alloc = std::allocator<T>>
void ConcurrencyVector<T, Alloc>::PushBack(T item) {
	std::unique_lock<std::mutex> lk(this->pushMtx);
	try {
		this->dataVector.push_back(item);
	}
	catch (...) {
		int stop = 324;
	}
	auto size = this->dataVector.size();
	if (size > this->nowWaiting)
	{
		this->pushCv.notify_one();
		notified = true;
	}
}

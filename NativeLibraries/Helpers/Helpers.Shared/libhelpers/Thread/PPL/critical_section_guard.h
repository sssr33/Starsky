#pragma once
#include "..\..\Metainfo\PointerGetter.h"
#include "..\..\Metainfo\RawType.h"
#include "critical_section_lock.h"

#include <ppl.h>
#include <algorithm>

template<class T>
class critical_section_guard_accessor{
public:
	critical_section_guard_accessor(Concurrency::critical_section &cs, T &obj)
		: lock(cs), obj(obj){
	}

	critical_section_guard_accessor(const critical_section_guard_accessor &other) = delete;

	critical_section_guard_accessor(critical_section_guard_accessor &&other)
		: lock(std::move(other.lock)), obj(std::move(other.obj)){
	}

	~critical_section_guard_accessor(){
	}

	critical_section_guard_accessor &operator=(const critical_section_guard_accessor &other) = delete;

	critical_section_guard_accessor &operator=(critical_section_guard_accessor &&other){
		if (this != &other){
			this->lock = std::move(other.lock);
			this->obj = std::move(other.obj);
		}

		return *this;
	}

	T *operator->(){
		return &this->obj;
	}

	const T *operator->() const{
		return &this->obj;
	}

	operator T*(){
		return &this->obj;
	}

	operator const T*() const{
		return &this->obj;
	}
private:
	critical_section_lock lock;
	T &obj;
};



// MultithreadWrapper must have constructor wchich accepts pointer to T
template<class T, class MultithreadWrapper = void>
class critical_section_guard{
public:
	typedef critical_section_guard_accessor<typename RawType<T>::Type> Accessor;

	template<class A1>
	critical_section_guard(const A1 &arg1)
		: obj(arg1){
	}

	template<class A1, class A2>
	critical_section_guard(const A1 &arg1, const A2 &arg2)
		: obj(arg1, arg2){
	}

	critical_section_guard(){
	}

	critical_section_guard(const T &obj)
		: obj(obj){
	}

	critical_section_guard(T &&obj)
		: obj(std::move(obj)){
	}

	~critical_section_guard(){
	}

	critical_section_guard_accessor<typename RawType<T>::Type> Get(){
		return critical_section_guard_accessor<typename RawType<T>::Type>(this->cs, *PointerGetter::Get(this->obj));
	}

	MultithreadWrapper GetMultithread() {
		return MultithreadWrapper(PointerGetter::Get(this->obj));
	}
private:
	Concurrency::critical_section cs;
	T obj;
};

template<class T>
class critical_section_guard<T, void> {
public:
	typedef critical_section_guard_accessor<typename RawType<T>::Type> Accessor;

	template<class A1>
	critical_section_guard(const A1 &arg1)
		: obj(arg1) {
	}

	template<class A1, class A2>
	critical_section_guard(const A1 &arg1, const A2 &arg2)
		: obj(arg1, arg2) {
	}

	critical_section_guard() {
	}

	critical_section_guard(const T &obj)
		: obj(obj) {
	}

	critical_section_guard(T &&obj)
		: obj(std::move(obj)) {
	}

	~critical_section_guard() {
	}

	critical_section_guard_accessor<typename RawType<T>::Type> Get() {
		return critical_section_guard_accessor<typename RawType<T>::Type>(this->cs, *PointerGetter::Get(this->obj));
	}

private:
	Concurrency::critical_section cs;
	T obj;
};
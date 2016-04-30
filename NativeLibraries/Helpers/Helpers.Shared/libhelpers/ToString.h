#pragma once
#include "config.h"
#include "HMath.h"

#include <d2d1.h>
#include <DirectXMath.h>
#include <string>

template<class T> struct ToStringHelper {
	static std::string Do(const T &v) {
		static_assert(false, "Not implemented");
	}
};

struct ToStr {
	template<class T> static std::string Do(const T &v) {
		return ToStringHelper<T>::Do(v);
	}
};

template<> struct ToStringHelper<DirectX::XMFLOAT2> {
	static std::string Do(const DirectX::XMFLOAT2 &v) {
		std::string str =
			"{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + "}";
		return str;
	}
};

template<> struct ToStringHelper<DirectX::XMFLOAT3> {
	static std::string Do(const DirectX::XMFLOAT3 &v) {
		std::string str =
			"{ " + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + "}";
		return str;
	}
};

template<> struct ToStringHelper<DirectX::XMVECTOR>{
	static std::string Do(const DirectX::XMVECTOR &v) {
		std::string str = 
			"{ " + std::to_string(v.XF) + ", " + std::to_string(v.YF) + ", " + std::to_string(v.ZF) + ", " + std::to_string(v.WF) + "}";
		return str;
	}
};

template<> struct ToStringHelper<DirectX::XMMATRIX> {
	static std::string Do(const DirectX::XMMATRIX &v) {
		std::string str = 
			"{\n" + ToStr::Do(v.r[0]) + "\n" + ToStr::Do(v.r[1]) + "\n" + ToStr::Do(v.r[2]) + "\n" + ToStr::Do(v.r[3]) + "\n}";
		return str;
	}
};

template<> struct ToStringHelper<D2D1::Matrix3x2F> {
	static std::string Do(const D2D1::Matrix3x2F &v) {
		std::string str = "{\n";
		str += ToStr::Do(DirectX::XMFLOAT2(v._11, v._12)) + "\n";
		str += ToStr::Do(DirectX::XMFLOAT2(v._21, v._22)) + "\n";
		str += ToStr::Do(DirectX::XMFLOAT2(v._31, v._32));
		str += "\n}";
		return str;
	}
};
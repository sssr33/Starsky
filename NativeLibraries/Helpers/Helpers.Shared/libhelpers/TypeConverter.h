#pragma once
#include "config.h"

#include <d2d1.h>
#include <string>

template<class D, class S>
struct TypeConverter {
	static D Convert(const S &v) {
		static_assert(false, "Not implemented");
		return D();
	}
};

#if HAVE_WINRT == 1
// Windows::UI::Color <-> D2D1::ColorF
template<>
struct TypeConverter<Windows::UI::Color, D2D1::ColorF> {
	static Windows::UI::Color Convert(const D2D1::ColorF &v) {
		Windows::UI::Color color;

		color.A = (uint8_t)(v.a * 255.0f);
		color.R = (uint8_t)(v.r * 255.0f);
		color.G = (uint8_t)(v.g * 255.0f);
		color.B = (uint8_t)(v.b * 255.0f);

		return color;
	}
};

template<>
struct TypeConverter<D2D1::ColorF, Windows::UI::Color> {
	static D2D1::ColorF Convert(const Windows::UI::Color &v) {
		const float Scale = 1.0f / 255.0f;
		float a, r, g, b;

		a = (float)v.A * Scale;
		r = (float)v.R * Scale;
		g = (float)v.G * Scale;
		b = (float)v.B * Scale;

		auto compColor = Windows::UI::Colors::Transparent;
		if (v.A == compColor.A && v.R == compColor.R && v.G == compColor.G && v.B == compColor.B) {
			a = r = g = b = 0.0f;
		}

		return D2D1::ColorF(r, g, b, a);
	}
};

template<>
struct TypeConverter<D2D1_COLOR_F, D2D1::ColorF> {
	static D2D1_COLOR_F Convert(const D2D1::ColorF &v) {
		D2D1_COLOR_F color;

		color.a = v.a;
		color.r = v.r;
		color.g = v.g;
		color.b = v.b;

		return color;
	}
};

template<>
struct TypeConverter<D2D1::ColorF, D2D1_COLOR_F> {
	static D2D1::ColorF Convert(const D2D1_COLOR_F &v) {
		D2D1::ColorF color(v.r, v.g, v.b, v.a);
		return color;
	}
};

// Platform::String ^ <-> std::wstring
template<>
struct TypeConverter<Platform::String ^, std::wstring> {
	static Platform::String ^Convert(const std::wstring &v) {
		Platform::String ^tmp = ref new Platform::String(v.data(), v.size());
		return tmp;
	}
};

template<>
struct TypeConverter<std::wstring, Platform::String ^> {
	static std::wstring Convert(Platform::String ^v) {
		std::wstring tmp(v->Data(), v->Length());
		return tmp;
	}
};

// Platform::String ^ <-> std::string
template<>
struct TypeConverter<Platform::String ^, std::string> {
	static Platform::String ^Convert(const std::string &v) {
		std::wstring tmpWStr(v.begin(), v.end());
		Platform::String ^tmp = ref new Platform::String(tmpWStr.data(), tmpWStr.size());
		return tmp;
	}
};

template<>
struct TypeConverter<std::string, Platform::String ^> {
	static std::string Convert(Platform::String ^v) {
		std::wstring tmpWStr(v->Data(), v->Length());
		std::string tmp(tmpWStr.begin(), tmpWStr.end());
		return tmp;
	}
};

// std::wstring <-> std::string
template<>
struct TypeConverter<std::wstring, std::string> {
	static std::wstring Convert(const std::string &v) {
		std::wstring tmp(v.begin(), v.end());
		return tmp;
	}
};

template<>
struct TypeConverter<std::string, std::wstring> {
	static std::string Convert(const std::wstring &v) {
		std::string tmp(v.begin(), v.end());
		return tmp;
	}
};

// D2D1_SIZE_U -> D2D1_SIZE_F
template<>
struct TypeConverter<D2D1_SIZE_F, D2D1_SIZE_U> {
	static D2D1_SIZE_F Convert(const D2D1_SIZE_U &v) {
		D2D1_SIZE_F tmp;

		tmp.width = (float)v.width;
		tmp.height = (float)v.height;

		return tmp;
	}
};
#endif
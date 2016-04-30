#pragma once
#include "config.h"
#include "HData.h"

#include <windows.h>
#include <DirectXMath.h>

struct GUIDHash {
	std::size_t operator()(const GUID &v) const {
		const uint64_t *tmpData4 = reinterpret_cast<const uint64_t *>(v.Data4);

		std::size_t h1 = std::hash<unsigned long>()(v.Data1);
		std::size_t h2 = std::hash<unsigned short>()(v.Data2);
		std::size_t h3 = std::hash<unsigned short>()(v.Data3);
		std::size_t h4 = std::hash<uint64_t>()(*tmpData4);

		return HData::CombineHash(h1, HData::CombineHash(h2, HData::CombineHash(h3, h4)));
	}
};

struct XMUINT2Hash {
	std::size_t operator()(const DirectX::XMUINT2 &v) const {
		std::size_t h1 = std::hash<unsigned long>()(v.x);
		std::size_t h2 = std::hash<unsigned short>()(v.y);

		return HData::CombineHash(h1, h2);
	}
};
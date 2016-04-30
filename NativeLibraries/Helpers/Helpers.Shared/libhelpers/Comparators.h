#pragma once
#include "config.h"

#include <Windows.h>
#include <DirectXMath.h>

struct XMUINT2Less {
	bool operator()(const DirectX::XMUINT2 &a, const DirectX::XMUINT2 &b) const {
		if (a.y == b.y) {
			return a.x < b.x;
		}
		else {
			return a.y < b.y;
		}
	}
};

struct XMUINT3Less {
	bool operator()(const DirectX::XMUINT3 &a, const DirectX::XMUINT3 &b) const {
		if (a.z == b.z) {
			if (a.y == b.y) {
				return a.x < b.x;
			}
			else {
				return a.y < b.y;
			}
		}
		else {
			return a.z < b.z;
		}
	}
};

struct XMUINT3Equal {
	bool operator()(const DirectX::XMUINT3 &a, const DirectX::XMUINT3 &b) const {
		bool equ = 
			a.x == b.x && 
			a.y == b.y && 
			a.z == b.z;

		return equ;
	}
};
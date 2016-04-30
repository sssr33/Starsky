#pragma once
#include "HText.h"
#include "HData.h"
#include "HSystem.h"
#include "HTime.h"
#include "HMath.h"

#include "WinRT/HDataWinRT.h"
#include "Macros.h"

class H{
public:
	typedef HText Text;
	typedef HData Data;
	typedef HSystem System;
	typedef HTime Time;
	typedef HMath Math;

#if HAVE_WINRT == 1
	class WinRT{
	public:
		typedef HDataWinRT Data;
	};
#endif
};
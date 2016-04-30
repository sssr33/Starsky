#pragma once
#include "Dx\Dx.h"

#include <libhelpers\raw_ptr.h>

class WinRtRenderer {
public:
	WinRtRenderer();
	virtual ~WinRtRenderer();

	raw_ptr<DxDevice> GetDxDev();

protected:
	DxDevice dxDev;
};
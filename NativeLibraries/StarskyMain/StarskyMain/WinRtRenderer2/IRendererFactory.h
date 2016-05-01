#pragma once
#include "Dx\Dx.h"
#include "IOutput.h"
#include "IRenderer.h"

#include <libhelpers\raw_ptr.h>

class IRendererFactory {
public:
	IRendererFactory();
	virtual ~IRendererFactory();

	virtual IRenderer *CreateRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output) = 0;
};
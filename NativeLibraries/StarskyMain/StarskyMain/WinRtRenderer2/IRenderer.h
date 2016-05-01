#pragma once
#include "IOutput.h"
#include "Dx\Dx.h"

#include <libhelpers\raw_ptr.h>

class IRenderer {
public:
	IRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output);
	virtual ~IRenderer();

	virtual void Render() = 0;
	virtual void OutputParametersChanged() = 0;
	virtual void Input() = 0;

protected:
	raw_ptr<DxDevice> dxDev;
	raw_ptr<IOutput> output;
};
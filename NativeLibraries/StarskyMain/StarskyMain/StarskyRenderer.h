#pragma once
#include "WinRtRenderer2\IRenderer.h"

class StarskyRenderer : public IRenderer {
public:
	StarskyRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output);
	virtual ~StarskyRenderer();

	virtual void Render() override;
	virtual void OutputParametersChanged() override;
	virtual void Input() override;

private:
};
#pragma once
#include "Dx\Dx.h"

class IOutput {
public:
	IOutput();
	virtual ~IOutput();

	virtual float GetLogicalDpi() const = 0;
	virtual DirectX::XMFLOAT2 GetLogicalSize() const = 0;
	virtual D3D11_VIEWPORT GetD3DViewport() const = 0;
	virtual ID3D11RenderTargetView *GetD3DRtView() const = 0;
	virtual ID2D1Bitmap1 *GetD2DRtView() const = 0;
};
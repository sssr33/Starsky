#pragma once
#include "IOutput.h"
#include "Dx\Dx.h"

#include <libhelpers\raw_ptr.h>

class SwapChainPanelOutput2 : public IOutput {
	static const uint32_t BufferCount = 2;
	static const DXGI_FORMAT BufferFmt = DXGI_FORMAT_B8G8R8A8_UNORM;
public:
	SwapChainPanelOutput2(
		raw_ptr<DxDevice> dxDev,
		Windows::UI::Xaml::Controls::SwapChainPanel ^swapChainPanel);
	virtual ~SwapChainPanelOutput2();

	D3D11_VIEWPORT GetScreenViewport() const;
	ID3D11RenderTargetView *GetRtView();

	void Present();

	void BeginRender();
	void EndRender();

private:
	raw_ptr<DxDevice> dxDev;
	Windows::UI::Xaml::Controls::SwapChainPanel ^swapChainPanel;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3dRenderTargetView;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2dTargetBitmap;

	Windows::Graphics::Display::DisplayOrientations nativeOrientation;
	Windows::Graphics::Display::DisplayOrientations currentOrientation;

	float logicalDpi;
	DirectX::XMFLOAT2 logicalSize;
	DirectX::XMFLOAT2 physicalSize;
	DirectX::XMFLOAT2 compositionScale;

	void CreateWindowSizeDependentResources();
	void CreateSwapChain();

	void UpdatePresentationParameters();
	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
	float ConvertDipsToPixels(float dips, float dpi);
};
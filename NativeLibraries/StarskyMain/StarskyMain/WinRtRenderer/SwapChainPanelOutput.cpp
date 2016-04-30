#include "pch.h"
#include "SwapChainPanelOutput.h"

#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <windows.ui.xaml.media.dxinterop.h>
#include <libhelpers\H.h>

SwapChainPanelOutput::SwapChainPanelOutput(
	raw_ptr<DxDevice> dxDev,
	Windows::UI::Xaml::Controls::SwapChainPanel ^swapChainPanel)
	: dxDev(dxDev), swapChainPanel(swapChainPanel), physicalSize(1.0f, 1.0f)
{
	auto displayInformation = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

	this->nativeOrientation = displayInformation->NativeOrientation;
	this->currentOrientation = displayInformation->CurrentOrientation;

	this->logicalDpi = displayInformation->LogicalDpi;
	this->logicalSize = DirectX::XMFLOAT2(
		(float)this->swapChainPanel->ActualWidth,
		(float)this->swapChainPanel->ActualHeight);
	this->compositionScale = DirectX::XMFLOAT2(
		this->swapChainPanel->CompositionScaleX,
		this->swapChainPanel->CompositionScaleY);

	this->CreateWindowSizeDependentResources();
}

D3D11_VIEWPORT SwapChainPanelOutput::GetScreenViewport() const {
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = this->logicalSize.x;
	viewport.Height = this->logicalSize.y;

	return viewport;
}

ID3D11RenderTargetView *SwapChainPanelOutput::GetRtView() {
	return this->d3dRenderTargetView.Get();
}

void SwapChainPanelOutput::Present() {
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = this->swapChain->Present(1, 0);

	{
		auto ctx = this->dxDev->GetContext();

		// Discard the contents of the render target.
		// This is a valid operation only when the existing contents will be entirely
		// overwritten. If dirty or scroll rects are used, this call should be modified.
		ctx->D3D()->DiscardView1(this->d3dRenderTargetView.Get(), nullptr, 0);
	}

	//// If the device was removed either by a disconnection or a driver upgrade, we 
	//// must recreate all device resources.
	//if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	//{
	//	HandleDeviceLost();
	//}
	//else
	//{
	//	DX::ThrowIfFailed(hr);
	//}
}

void SwapChainPanelOutput::CreateWindowSizeDependentResources() {
	// Clear the previous window size specific context.
	HRESULT hr = S_OK;
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
	auto d3dDev = this->dxDev->GetD3DDevice();
	auto d2dCtxMt = this->dxDev->GetD2DCtxMt();
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(SwapChainPanelOutput::BufferFmt, D2D1_ALPHA_MODE_PREMULTIPLIED),
			this->logicalDpi,
			this->logicalDpi
			);

	this->d3dRenderTargetView = nullptr;
	this->d2dTargetBitmap = nullptr;

	{
		auto ctx = this->dxDev->GetContext();

		ctx->D2D()->SetTarget(nullptr);
		ctx->D3D()->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
		ctx->D3D()->Flush();
	}

	this->UpdatePresentationParameters();

	if (this->swapChain) {
		this->swapChain->ResizeBuffers(
			SwapChainPanelOutput::BufferCount, 
			lround(this->physicalSize.x),
			lround(this->physicalSize.y),
			SwapChainPanelOutput::BufferFmt, 
			0);
	}
	else {
		this->CreateSwapChain();
	}

	hr = this->swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
	H::System::ThrowIfFailed(hr);

	hr = this->swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	H::System::ThrowIfFailed(hr);

	hr = d3dDev->CreateRenderTargetView(
		backBuffer.Get(), 
		nullptr, 
		this->d3dRenderTargetView.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	hr = d2dCtxMt->CreateBitmapFromDxgiSurface(
		dxgiBackBuffer.Get(), 
		bitmapProperties, 
		this->d2dTargetBitmap.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	Microsoft::WRL::ComPtr<IDXGISwapChain2> swapChain2;

	hr = this->swapChain.As(&swapChain2);
	H::System::ThrowIfFailed(hr);

	DXGI_MATRIX_3X2_F swapchainTransform = { 0 };

	swapchainTransform._11 = 1.0f / this->compositionScale.x;
	swapchainTransform._22 = 1.0f / this->compositionScale.y;

	hr = swapChain2->SetMatrixTransform(&swapchainTransform);
	H::System::ThrowIfFailed(hr);

	{
		auto viewport = this->GetScreenViewport();
		auto ctx = this->dxDev->GetContext();

		ctx->D3D()->RSSetViewports(1, &viewport);

		ctx->D2D()->SetTarget(this->d2dTargetBitmap.Get());
		ctx->D2D()->SetDpi(this->logicalDpi, this->logicalDpi);
		ctx->D2D()->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
	}
}

void SwapChainPanelOutput::CreateSwapChain() {
	// Otherwise, create a new one using the same adapter as the existing Direct3D device.
	HRESULT hr = S_OK;
	DXGI_SCALING scaling = DXGI_SCALING_STRETCH;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice;
	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
	Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChainTmp;
	auto d3dDev = this->dxDev->GetD3DDeviceCPtr();

	swapChainDesc.Width = lround(this->physicalSize.x);		// Match the size of the window.
	swapChainDesc.Height = lround(this->physicalSize.y);
	swapChainDesc.Format = SwapChainPanelOutput::BufferFmt;				// This is the most common swap chain format.
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;								// Don't use multi-sampling.
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SwapChainPanelOutput::BufferCount;									// Use double-buffering to minimize latency.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// All Windows Store apps must use _FLIP_ SwapEffects.
	swapChainDesc.Flags = 0;
	swapChainDesc.Scaling = scaling;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

	hr = d3dDev.As(&dxgiDevice);
	H::System::ThrowIfFailed(hr);

	hr = dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
	H::System::ThrowIfFailed(hr);

	hr = dxgiFactory->CreateSwapChainForComposition(d3dDev.Get(), &swapChainDesc, nullptr, swapChainTmp.GetAddressOf());
	H::System::ThrowIfFailed(hr);

	hr = swapChainTmp.As(&this->swapChain);
	H::System::ThrowIfFailed(hr);

	// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
	// ensures that the application will only render after each VSync, minimizing power consumption.
	hr = dxgiDevice->SetMaximumFrameLatency(1);
	H::System::ThrowIfFailed(hr);

	// Associate swap chain with SwapChainPanel
	// UI changes will need to be dispatched back to the UI thread
	auto swapChainPanelTmp = this->swapChainPanel;
	auto swapChainTmp2 = this->swapChain;
	this->swapChainPanel->Dispatcher->RunAsync(
		Windows::UI::Core::CoreDispatcherPriority::High, 
		ref new Windows::UI::Core::DispatchedHandler([swapChainPanelTmp, swapChainTmp2]()
	{
		// Get backing native interface for SwapChainPanel
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ISwapChainPanelNative> panelNative;
		auto swapChainPanelUnk = reinterpret_cast<IUnknown *>(swapChainPanelTmp);

		hr = swapChainPanelUnk->QueryInterface(IID_PPV_ARGS(&panelNative));
		H::System::ThrowIfFailed(hr);

		hr = panelNative->SetSwapChain(swapChainTmp2.Get());
		H::System::ThrowIfFailed(hr);
	}, Platform::CallbackContext::Any));
}

void SwapChainPanelOutput::UpdatePresentationParameters() {
	this->logicalSize.x = (std::max)(this->logicalSize.x, 1.0f);
	this->logicalSize.y = (std::max)(this->logicalSize.y, 1.0f);

	this->physicalSize.x = this->ConvertDipsToPixels(this->logicalSize.x, this->logicalDpi);
	this->physicalSize.y = this->ConvertDipsToPixels(this->logicalSize.y, this->logicalDpi);
}

float SwapChainPanelOutput::ConvertDipsToPixels(float dips, float dpi) {
	static const float dipsPerInch = 96.0f;
	return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
}
#pragma once
#include "Dx\Dx.h"
#include "IRendererFactory.h"
#include "SwapChainPanelOutput2.h"

#include <memory>
#include <thread>

class WinRtRenderer2 {
public:
	WinRtRenderer2(
		Windows::UI::Xaml::Controls::SwapChainPanel ^panel,
		IRendererFactory &factory);
	~WinRtRenderer2();

private:
	DxDevice dxDev;
	SwapChainPanelOutput2 output;
	std::unique_ptr<IRenderer> renderer;

	Windows::UI::Core::CoreIndependentInputSource ^coreInput;

	std::thread renderThread;
	std::thread inputThread;

	void Input(Windows::UI::Xaml::Controls::SwapChainPanel ^panel);
	void Render();
};
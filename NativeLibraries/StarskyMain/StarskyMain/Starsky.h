#pragma once
#include "StarskyWinRtRenderer.h"
#include "WinRtRenderer\WinRtRendererMain.h"

namespace StarskyMain {
	[Windows::Foundation::Metadata::WebHostHiddenAttribute]
	public ref class Starsky sealed {
	public:
		Starsky(Windows::UI::Xaml::Controls::SwapChainPanel ^panel);
		virtual ~Starsky();

	private:
		WinRtRendererMain<StarskyWinRtRenderer> renderer;
	};
}
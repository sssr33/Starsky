#pragma once
#include "StarskyRenderer.h"
#include "WinRtRenderer2\WinRtRenderer2.h"

namespace StarskyMain {
	[Windows::Foundation::Metadata::WebHostHiddenAttribute]
	public ref class Starsky sealed {
	public:
		Starsky(Windows::UI::Xaml::Controls::SwapChainPanel ^panel);
		virtual ~Starsky();

	private:
		WinRtRenderer2<StarskyRenderer> renderer;
	};
}
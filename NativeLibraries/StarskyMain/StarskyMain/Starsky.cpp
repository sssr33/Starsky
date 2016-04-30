#include "pch.h"
#include "Starsky.h"

using namespace StarskyMain;

Starsky::Starsky(Windows::UI::Xaml::Controls::SwapChainPanel ^panel)
	: renderer(panel)
{
}

Starsky::~Starsky() {
}
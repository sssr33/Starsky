#include "pch.h"
#include "WinRtRenderer2.h"

WinRtRenderer2::WinRtRenderer2(
	Windows::UI::Xaml::Controls::SwapChainPanel ^panel,
	IRendererFactory &factory)
	: output(&this->dxDev, panel),
	renderer(factory.CreateRenderer(&this->dxDev, &this->output))
{
	auto window = Windows::UI::Xaml::Window::Current->CoreWindow;
	auto displayInformation = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

	window->VisibilityChanged += H::System::MakeTypedEventHandler(
		[=](Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^args)
	{
		int stop = 324;
	});

	displayInformation->DpiChanged += H::System::MakeTypedEventHandler(
		[=](Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
	{
		int stop = 324;
	});

	displayInformation->OrientationChanged += H::System::MakeTypedEventHandler(
		[=](Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
	{
		int stop = 324;
	});

	displayInformation->DisplayContentsInvalidated += H::System::MakeTypedEventHandler(
		[=](Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
	{
		int stop = 324;
	});

	panel->CompositionScaleChanged += H::System::MakeTypedEventHandler(
		[=](Windows::UI::Xaml::Controls::SwapChainPanel ^sender, Platform::Object ^args)
	{
		int stop = 324;
	});

	panel->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(
		[=](Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^args)
	{
		int stop = 324;
	});

	this->renderer->OutputParametersChanged();

	this->renderThread = std::thread([=]() { this->Render(); });
	this->inputThread = std::thread([=]() { this->Input(panel); });
}

WinRtRenderer2::~WinRtRenderer2() {
	if (this->renderThread.joinable()) {
		this->renderThread.join();
	}

	if (this->inputThread.joinable()) {
		this->inputThread.join();
	}
}

void WinRtRenderer2::Input(Windows::UI::Xaml::Controls::SwapChainPanel ^panel) {
	this->coreInput = panel->CreateCoreIndependentInputSource(
		Windows::UI::Core::CoreInputDeviceTypes::Mouse |
		Windows::UI::Core::CoreInputDeviceTypes::Touch |
		Windows::UI::Core::CoreInputDeviceTypes::Pen);

	this->coreInput->PointerPressed += H::System::MakeTypedEventHandler(
		[=](Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) { });
	this->coreInput->PointerMoved += H::System::MakeTypedEventHandler(
		[=](Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) { });
	this->coreInput->PointerReleased += H::System::MakeTypedEventHandler(
		[=](Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) { });

	this->coreInput->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessUntilQuit);
}

void WinRtRenderer2::Render() {
	while (true) {
		this->output.BeginRender();

		this->renderer->Render();

		this->output.EndRender();
	}
}
#pragma once
#include "Dx\Dx.h"
#include "SwapChainPanelOutput2.h"

#include <memory>
#include <thread>
#include <type_traits>
#include <libhelpers\Thread\critical_section.h>

template<class T>
class WinRtRenderer2 {
public:
	WinRtRenderer2(
		Windows::UI::Xaml::Controls::SwapChainPanel ^panel)
		: output(&this->dxDev, panel),
		renderer(&this->dxDev, &this->output), renderWorking(true)
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
			auto newSizeTmp = DirectX::XMFLOAT2(args->NewSize.Width, args->NewSize.Height);
			thread::critical_section::scoped_lock lk(this->lk);

			this->output.Resize(newSizeTmp);
			this->renderer.OutputParametersChanged();
		});

		this->renderer.OutputParametersChanged();

		this->renderThread = std::thread([=]() { this->Render(); });
		this->inputThread = std::thread([=]() { this->Input(panel); });
	}

	~WinRtRenderer2() {
		static_assert(std::is_base_of<IRenderer, T>::value, "Renderer must inherit from IRenderer");

		if (this->renderThread.joinable()) {
			this->renderThread.join();
		}

		if (this->inputThread.joinable()) {
			this->inputThread.join();
		}
	}

private:
	DxDevice dxDev;
	SwapChainPanelOutput2 output;
	T renderer;

	thread::critical_section lk;

	Windows::UI::Core::CoreIndependentInputSource ^coreInput;

	std::thread renderThread;
	std::thread inputThread;

	bool renderWorking;

	void Input(Windows::UI::Xaml::Controls::SwapChainPanel ^panel) {
		this->coreInput = panel->CreateCoreIndependentInputSource(
			Windows::UI::Core::CoreInputDeviceTypes::Mouse |
			Windows::UI::Core::CoreInputDeviceTypes::Touch |
			Windows::UI::Core::CoreInputDeviceTypes::Pen);

		this->coreInput->PointerPressed += H::System::MakeTypedEventHandler(
			[=](Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) {});
		this->coreInput->PointerMoved += H::System::MakeTypedEventHandler(
			[=](Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) {});
		this->coreInput->PointerReleased += H::System::MakeTypedEventHandler(
			[=](Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) {});

		this->coreInput->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessUntilQuit);
	}

	void Render() {
		while (this->renderWorking) {
			thread::critical_section::scoped_yield_lock lk(this->lk);

			this->output.BeginRender();
			this->renderer.Render();
			this->output.EndRender();
		}
	}
};
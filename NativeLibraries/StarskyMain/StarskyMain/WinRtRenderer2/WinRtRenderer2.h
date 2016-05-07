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

		displayInformation->DpiChanged += H::System::MakeTypedEventHandler(
			[=](Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
		{
			thread::critical_section::scoped_lock lk(this->lk);
			auto panel = this->output.GetSwapChainPanel();
			auto newDpi = sender->LogicalDpi;
			auto newScale = DirectX::XMFLOAT2(panel->CompositionScaleX, panel->CompositionScaleY);
			auto newSize = DirectX::XMFLOAT2((float)panel->ActualWidth, (float)panel->ActualHeight);

			this->TryResize(newSize, newScale, newDpi);
		});

		panel->CompositionScaleChanged += H::System::MakeTypedEventHandler(
			[=](Windows::UI::Xaml::Controls::SwapChainPanel ^sender, Platform::Object ^args)
		{
			auto displayInformation = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

			thread::critical_section::scoped_lock lk(this->lk);
			auto newDpi = displayInformation->LogicalDpi;
			auto newScale = DirectX::XMFLOAT2(sender->CompositionScaleX, sender->CompositionScaleY);
			auto newSize = DirectX::XMFLOAT2((float)sender->ActualWidth, (float)sender->ActualHeight);

			this->TryResize(newSize, newScale, newDpi);
		});

		panel->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(
			[=](Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^args)
		{
			auto displayInformation = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
			auto newSize = DirectX::XMFLOAT2(args->NewSize.Width, args->NewSize.Height);

			thread::critical_section::scoped_lock lk(this->lk);
			auto panel = this->output.GetSwapChainPanel();
			auto newDpi = displayInformation->LogicalDpi;
			auto newScale = DirectX::XMFLOAT2(panel->CompositionScaleX, panel->CompositionScaleY);

			this->TryResize(newSize, newScale, newDpi);
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

	// Need to inspect all parameters in order to use correct size parameters under windows 8.1
	// Because under 8.1 it isn't reports events about all changes, for example it refers to DPI
	void TryResize(const DirectX::XMFLOAT2 &newSize, const DirectX::XMFLOAT2 &newScale, float dpi) {
		bool needResize = false;
		auto oldDpi = this->output.GetLogicalDpi();
		auto oldScale = this->output.GetCompositionScale();
		auto oldSize = this->output.GetLogicalSize();

		if (dpi != oldDpi) {
			needResize = true;
			this->output.SetLogicalDpi(dpi);
		}

		if (newScale != oldScale) {
			needResize = true;
			this->output.SetCompositionScale(newScale);
		}

		if (newSize != oldSize) {
			needResize = true;
			this->output.SetLogicalSize(newSize);
		}

		if (needResize) {
			this->output.Resize();
			this->renderer.OutputParametersChanged();
		}
	}
};
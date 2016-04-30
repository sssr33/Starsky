#pragma once
#include "SwapChainPanelOutput.h"

#include <thread>
#include <libhelpers\H.h>

template<class T>
class WinRtRendererMain {
public:
	template<typename... RendererArgs>
	WinRtRendererMain(Windows::UI::Xaml::Controls::SwapChainPanel ^panel,
		RendererArgs&& ...args)
		: renderer(std::forward<RendererArgs>(args)...),
		dxDev(renderer.GetDxDev()),
		output(dxDev, panel),
		renderThread([=]() { this->Render(); }),
		inputThread([=]() { this->Input(panel); })
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
	}

	~WinRtRendererMain() {
		if (this->renderThread.joinable()) {
			this->renderThread.join();
		}

		if (this->inputThread.joinable()) {
			this->inputThread.join();
		}
	}

private:
	T renderer;
	raw_ptr<DxDevice> dxDev;
	SwapChainPanelOutput output;

	Windows::UI::Core::CoreIndependentInputSource ^coreInput;

	std::thread renderThread;
	std::thread inputThread;

	void Render() {
		while (true) {
			{
				auto rtView = this->output.GetRtView();
				ID3D11RenderTargetView *const targets[1] = { this->output.GetRtView() };
				auto ctx = this->dxDev->GetContext();

				ctx->D3D()->OMSetRenderTargets(1, targets, nullptr);

				ctx->D3D()->ClearRenderTargetView(this->output.GetRtView(), DirectX::Colors::CornflowerBlue);
			}


			this->output.Present();
		}
	}

	void Input(Windows::UI::Xaml::Controls::SwapChainPanel ^panel) {
		this->coreInput = panel->CreateCoreIndependentInputSource(
			Windows::UI::Core::CoreInputDeviceTypes::Mouse |
			Windows::UI::Core::CoreInputDeviceTypes::Touch |
			Windows::UI::Core::CoreInputDeviceTypes::Pen);

		this->coreInput->PointerPressed += H::System::MakeTypedEventHandler(
			[=](Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) { this->OnPointerPressed(sender, args); });
		this->coreInput->PointerMoved += H::System::MakeTypedEventHandler(
			[=](Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) { this->OnPointerMoved(sender, args); });
		this->coreInput->PointerReleased += H::System::MakeTypedEventHandler(
			[=](Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) { this->OnPointerReleased(sender, args); });

		this->coreInput->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessUntilQuit);
	}

	void OnPointerPressed(Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) {

	}

	void OnPointerMoved(Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) {

	}

	void OnPointerReleased(Platform::Object ^sender, Windows::UI::Core::PointerEventArgs ^args) {

	}
};
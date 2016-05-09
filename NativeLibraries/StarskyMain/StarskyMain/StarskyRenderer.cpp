#include "pch.h"
#include "StarskyRenderer.h"

StarskyRenderer::StarskyRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output)
	: IRenderer(dxDev, output)
{
}

StarskyRenderer::~StarskyRenderer() {
}

void StarskyRenderer::Render() {
	auto ctx = this->dxDev->GetContext();
	auto logicalSize = this->output->GetLogicalSize();

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;

	ctx->D2D()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), brush.GetAddressOf());

	ctx->D2D()->BeginDraw();
	
	ctx->D2D()->FillRectangle(D2D1::RectF(0, 0, 100, 100), brush.Get());
	ctx->D2D()->FillRectangle(D2D1::RectF(logicalSize.x - 100, logicalSize.y - 100, logicalSize.x, logicalSize.y), brush.Get());

	ctx->D2D()->DrawLine(D2D1::Point2F(0.0f, 200.0f), D2D1::Point2F(100.0f, 200.0f), brush.Get());

	ctx->D2D()->EndDraw();
}

void StarskyRenderer::OutputParametersChanged() {
}

void StarskyRenderer::Input() {
}
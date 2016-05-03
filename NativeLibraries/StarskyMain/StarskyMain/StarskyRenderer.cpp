#include "pch.h"
#include "StarskyRenderer.h"

StarskyRenderer::StarskyRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output)
	: IRenderer(dxDev, output)
{
}

StarskyRenderer::~StarskyRenderer() {
}

void StarskyRenderer::Render() {
}

void StarskyRenderer::OutputParametersChanged() {
}

void StarskyRenderer::Input() {
}
#include "pch.h"
#include "WinRtRenderer.h"

WinRtRenderer::WinRtRenderer() {
}

WinRtRenderer::~WinRtRenderer() {
}

raw_ptr<DxDevice> WinRtRenderer::GetDxDev() {
	return &this->dxDev;
}
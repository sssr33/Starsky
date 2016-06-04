#pragma once
#include "WinRtRenderer2\IRenderer.h"

struct AntialiasData {
	DirectX::XMFLOAT2 pt0;
	DirectX::XMFLOAT2 pt1;
	DirectX::XMFLOAT2 curPt;
};

class StarskyRenderer : public IRenderer {
public:
	StarskyRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output);
	virtual ~StarskyRenderer();

	virtual void Render() override;
	virtual void OutputParametersChanged() override;
	virtual void Input() override;

private:
	DirectX::XMFLOAT4X4 projection;

	std::vector<DirectX::XMFLOAT2> antialiasData;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mainGeometry;
	Microsoft::WRL::ComPtr<ID3D11Buffer> antialiasGeometry;
	Microsoft::WRL::ComPtr<ID3D11Buffer> antialiasDataBuf;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout2;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vsCBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> psCBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs2;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps2;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsState;
};
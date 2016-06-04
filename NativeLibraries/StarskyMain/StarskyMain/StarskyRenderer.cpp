#include "pch.h"
#include "StarskyRenderer.h"

#include <libhelpers\H.h>

StarskyRenderer::StarskyRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output)
	: IRenderer(dxDev, output)
{
	HRESULT hr = S_OK;
	auto d3dDev = dxDev->GetD3DDevice();

	{
		D3D11_BUFFER_DESC bufDesc;

		bufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.MiscFlags = 0;
		bufDesc.StructureByteStride = 0;

		hr = d3dDev->CreateBuffer(&bufDesc, nullptr, this->vsCBuffer.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		bufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4);

		hr = d3dDev->CreateBuffer(&bufDesc, nullptr, this->psCBuffer.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		D3D11_SUBRESOURCE_DATA bufData;

		DirectX::XMFLOAT2 geomData[] = {
			DirectX::XMFLOAT2(0.0f, 0.5f),
			DirectX::XMFLOAT2(0.5f, -0.5f),
			DirectX::XMFLOAT2(-0.5f, -0.5f)
		};

		bufDesc.ByteWidth = sizeof(geomData);
		bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		bufData.pSysMem = geomData;
		bufData.SysMemPitch = 0;
		bufData.SysMemSlicePitch = 0;

		hr = d3dDev->CreateBuffer(&bufDesc, &bufData, this->mainGeometry.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		for (size_t i = 0; i < ARRAY_SIZE(geomData) - 1; i++) {
			this->antialiasData.push_back(geomData[i]);
			this->antialiasData.push_back(geomData[i + 1]);
		}

		antialiasData.push_back(geomData[ARRAY_SIZE(geomData) - 1]);
		antialiasData.push_back(geomData[0]);

		bufDesc.ByteWidth = antialiasData.size() * sizeof(DirectX::XMFLOAT2);
		bufData.pSysMem = antialiasData.data();

		hr = d3dDev->CreateBuffer(&bufDesc, &bufData, this->antialiasGeometry.GetAddressOf());
		H::System::ThrowIfFailed(hr);


		bufDesc.ByteWidth = antialiasData.size() * sizeof(AntialiasData);
		bufDesc.Usage = D3D11_USAGE_DEFAULT;

		hr = d3dDev->CreateBuffer(&bufDesc, nullptr, this->antialiasDataBuf.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	{
		auto vsData = H::System::LoadPackageFile("StarskyMain\\SimpleVs.cso");
		auto psData = H::System::LoadPackageFile("StarskyMain\\SimplePs.cso");

		hr = d3dDev->CreateVertexShader(vsData.data(), vsData.size(), nullptr, this->vs.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		hr = d3dDev->CreatePixelShader(psData.data(), psData.size(), nullptr, this->ps.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		
		hr = d3dDev->CreateInputLayout(layoutDesc, ARRAY_SIZE(layoutDesc), vsData.data(), vsData.size(), this->inputLayout.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	{
		auto vsData = H::System::LoadPackageFile("StarskyMain\\SimpleAntialiasVs.cso");
		auto psData = H::System::LoadPackageFile("StarskyMain\\SimpleAntialiasPs.cso");

		hr = d3dDev->CreateVertexShader(vsData.data(), vsData.size(), nullptr, this->vs2.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		hr = d3dDev->CreatePixelShader(psData.data(), psData.size(), nullptr, this->ps2.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		hr = d3dDev->CreateInputLayout(layoutDesc, ARRAY_SIZE(layoutDesc), vsData.data(), vsData.size(), this->inputLayout2.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}

	{
		D3D11_RASTERIZER_DESC rastDesc;

		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.CullMode = D3D11_CULL_BACK;
		rastDesc.FrontCounterClockwise = FALSE;
		rastDesc.DepthBias = 0;
		rastDesc.SlopeScaledDepthBias = 0.0f;
		rastDesc.DepthBiasClamp = 0.0f;
		rastDesc.DepthClipEnable = TRUE;
		rastDesc.ScissorEnable = FALSE;
		rastDesc.MultisampleEnable = FALSE;
		rastDesc.AntialiasedLineEnable = FALSE;

		hr = d3dDev->CreateRasterizerState(&rastDesc, this->rsState.GetAddressOf());
		H::System::ThrowIfFailed(hr);
	}
}

StarskyRenderer::~StarskyRenderer() {
}

void StarskyRenderer::Render() {
	auto ctx = this->dxDev->GetContext();
	auto d2dCtx = ctx->D2D();
	auto d3dCtx = ctx->D3D();
	auto logicalSize = this->output->GetLogicalSize();
	auto d2dOrientation = this->output->GetD2DOrientationTransform();

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;

	d2dCtx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), brush.GetAddressOf());

	d2dCtx->SetTransform(d2dOrientation);
	d2dCtx->BeginDraw();
	
	d2dCtx->FillRectangle(D2D1::RectF(0, 0, 100, 100), brush.Get());
	d2dCtx->FillRectangle(D2D1::RectF(logicalSize.x - 100, logicalSize.y - 100, logicalSize.x, logicalSize.y), brush.Get());

	d2dCtx->DrawLine(D2D1::Point2F(0.0f, 200.0f), D2D1::Point2F(100.0f, 225.0f), brush.Get());

	d2dCtx->EndDraw();


	static float timer = 0.0f;
	static float scale = 1.0f;
	const float timerStep = 0.001f;

	scale = std::fabsf(1.0f - std::sinf(timer));

	timer += timerStep;

	auto proj = DirectX::XMLoadFloat4x4(&this->projection);

	auto transformObj = DirectX::XMMatrixIdentity();

	//transformObj = DirectX::XMMatrixMultiply(transformObj, DirectX::XMMatrixScaling(scale, scale, scale));
	//transformObj = DirectX::XMMatrixMultiply(transformObj, DirectX::XMMatrixRotationZ(timer));
	transformObj = DirectX::XMMatrixMultiply(transformObj, DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-22)));

	proj = DirectX::XMMatrixMultiply(transformObj, proj);
	
	auto transform = proj;
	proj = DirectX::XMMatrixTranspose(proj);
	d3dCtx->UpdateSubresource(this->vsCBuffer.Get(), 0, nullptr, &proj, 0, 0);

	DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	d3dCtx->UpdateSubresource(this->psCBuffer.Get(), 0, nullptr, &color, 0, 0);

	d3dCtx->IASetInputLayout(this->inputLayout.Get());
	d3dCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		uint32_t offset = 0;
		uint32_t stride = sizeof(DirectX::XMFLOAT2);

		d3dCtx->IASetVertexBuffers(0, 1, this->mainGeometry.GetAddressOf(), &stride, &offset);
	}

	d3dCtx->VSSetConstantBuffers(0, 1, this->vsCBuffer.GetAddressOf());
	d3dCtx->PSSetConstantBuffers(0, 1, this->psCBuffer.GetAddressOf());

	d3dCtx->VSSetShader(this->vs.Get(), nullptr, 0);
	d3dCtx->PSSetShader(this->ps.Get(), nullptr, 0);

	d3dCtx->Draw(3, 0);


	// line

	/*color.x = 1.0f;
	color.y = 0.0f;
	color.z = 0.0f;
	d3dCtx->UpdateSubresource(this->psCBuffer.Get(), 0, nullptr, &color, 0, 0);*/

	d3dCtx->IASetInputLayout(this->inputLayout2.Get());
	d3dCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	{
		uint32_t offset = 0;
		uint32_t stride = sizeof(DirectX::XMFLOAT2);

		d3dCtx->IASetVertexBuffers(0, 1, this->antialiasGeometry.GetAddressOf(), &stride, &offset);
	}

	{
		std::vector<AntialiasData> tmp;
		//std::vector<DirectX::XMFLOAT2> tmp;
		auto d3dViewport = this->output->GetD3DViewport();
		auto proj2 = transform;//DirectX::XMLoadFloat4x4(&this->projection);
		auto scaleM = DirectX::XMMatrixScaling(d3dViewport.Width / 2, d3dViewport.Height / 2, 1.0f);

		for (size_t i = 0; i < this->antialiasData.size(); i+=2) {
			AntialiasData res;
			auto pt0 = DirectX::XMLoadFloat2(&this->antialiasData[i]);
			auto pt1 = DirectX::XMLoadFloat2(&this->antialiasData[(i + 1) % this->antialiasData.size()]);

			/*pt0 = DirectX::XMVector3Project(
				pt0, 
				d3dViewport.TopLeftX, d3dViewport.TopLeftY, 
				d3dViewport.Width, d3dViewport.Height, 
				d3dViewport.MinDepth, d3dViewport.MaxDepth, 
				proj2, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());

			pt1 = DirectX::XMVector3Project(
				pt1,
				d3dViewport.TopLeftX, d3dViewport.TopLeftY,
				d3dViewport.Width, d3dViewport.Height,
				d3dViewport.MinDepth, d3dViewport.MaxDepth,
				proj2, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());*/

			pt0 = DirectX::XMVector3TransformCoord(pt0, proj2);
			pt1 = DirectX::XMVector3TransformCoord(pt1, proj2);

			pt0 = DirectX::XMVector3Transform(pt0, scaleM);
			pt1 = DirectX::XMVector3Transform(pt1, scaleM);

			DirectX::XMStoreFloat2(&res.pt0, pt0);
			DirectX::XMStoreFloat2(&res.pt1, pt1);

			res.curPt = res.pt0;
			tmp.push_back(res);
			//tmp.push_back(res.pt0);

			res.curPt = res.pt1;
			tmp.push_back(res);
			//tmp.push_back(res.pt1);
		}

		d3dCtx->UpdateSubresource(this->antialiasDataBuf.Get(), 0, nullptr, tmp.data(), 0, 0);
	}

	{
		uint32_t offset = 0;
		uint32_t stride = sizeof(AntialiasData);
		//uint32_t stride = sizeof(DirectX::XMFLOAT2);

		d3dCtx->IASetVertexBuffers(1, 1, this->antialiasDataBuf.GetAddressOf(), &stride, &offset);
	}

	d3dCtx->VSSetShader(this->vs2.Get(), nullptr, 0);
	d3dCtx->PSSetShader(this->ps2.Get(), nullptr, 0);

	//d3dCtx->RSSetState(this->rsState.Get());
	d3dCtx->Draw(6, 0);
}

void StarskyRenderer::OutputParametersChanged() {
	auto size = this->output->GetLogicalSize();
	float ar = (float)size.x / (float)size.y;

	auto proj = DirectX::XMMatrixOrthographicLH(2.0f * ar, 2.0f, 0.001f, 10.0f);
	DirectX::XMStoreFloat4x4(&this->projection, proj);
}

void StarskyRenderer::Input() {
}
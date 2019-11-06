#include "stdafx.h"
#include "fontShaderClass.h"

FontShaderClass::FontShaderClass()
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pInputLayout = nullptr;
	m_pConstantBuffer = nullptr;
	m_pSamplerState = nullptr;
	m_pPixelBuffer = nullptr;
}

FontShaderClass::FontShaderClass(const FontShaderClass&)
{
}

FontShaderClass::~FontShaderClass()
{
}

bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	return InitializeShader(device, hwnd, L"Shaders/font.vs", L"Shaders/font.ps");
}

void FontShaderClass::Shutdown()
{
	ShutdownShader();
}

bool FontShaderClass::Render(ID3D11DeviceContext* immediateContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatirx, XMMATRIX projMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{
	if (!SetShaderParameters(immediateContext, worldMatrix, viewMatirx, projMatrix, texture, pixelColor))
	{
		return false;
	}

	RenderShader(immediateContext, indexCount);

	return true;
}

bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ID3D10Blob* pErrorMsg = nullptr;

	ID3D10Blob* pVertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "FortVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pVertexShaderBuffer, &pErrorMsg)))
	{
		if (pErrorMsg)
		{
			OutputShaderErrorMessage(pErrorMsg, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK);
		}

		return false;
	}

	ID3D10Blob* pPixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pPixelShaderBuffer, &pErrorMsg)))
	{
		if (pErrorMsg)
		{
			OutputShaderErrorMessage(pErrorMsg, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK);
		}

		return false;
	}

	if (FAILED(device->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		return false;
	}

	if (FAILED(device->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader)))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	if (FAILED(device->CreateInputLayout(inputElementDesc, _countof(inputElementDesc), pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &m_pInputLayout)))
	{
		return false;
	}

	SAFE_RELEASE(pPixelShaderBuffer);
	SAFE_RELEASE(pVertexShaderBuffer);

	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&constantBufferDesc, NULL, &m_pConstantBuffer)))
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(device->CreateSamplerState(&samplerDesc, &m_pSamplerState)))
	{
		return false;
	}

	D3D11_BUFFER_DESC pixelBufferDesc;
	ZeroMemory(&pixelBufferDesc, sizeof(pixelBufferDesc));

	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&pixelBufferDesc, NULL, &m_pPixelBuffer)))
	{
		return false;
	}

	return true;
}

void FontShaderClass::ShutdownShader()
{
	SAFE_RELEASE(m_pPixelBuffer);
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexShader);
}

void FontShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilename)
{
	using namespace std;

	char* compileError;
	unsigned long bufferSize, i;
	ofstream fout;

	compileError = (char*)(errorMsg->GetBufferPointer());
	bufferSize = errorMsg->GetBufferSize();

	fout.open("ErrorCode/shader_error.txt");
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileError[i];
	}

	fout.close();

	SAFE_RELEASE(errorMsg);

	MessageBox(hwnd, L"Error compling shader. Check shaer-error.txt for message", shaderFilename, MB_OK);

	return;
}

bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* immediateContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));
	
	if (FAILED(immediateContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	ConstantBufferType* pConstantBuffer = (ConstantBufferType*)mappedResource.pData;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projMatrix = XMMatrixTranspose(projMatrix);

	pConstantBuffer->world = worldMatrix;
	pConstantBuffer->view = viewMatrix;
	pConstantBuffer->proj = projMatrix;

	immediateContext->Unmap(m_pConstantBuffer, 0);

	UINT iBufferNumber = 0;

	immediateContext->VSSetConstantBuffers(iBufferNumber, 1, &m_pConstantBuffer);
	immediateContext->PSSetShaderResources(0, 1, &texture);

	if (FAILED(immediateContext->Map(m_pPixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	PixelBufferType* pixelBuffer = (PixelBufferType*)mappedResource.pData;

	pixelBuffer->pixelColor = pixelColor;

	immediateContext->Unmap(m_pPixelBuffer, 0);

	iBufferNumber = 0;

	immediateContext->PSSetConstantBuffers(iBufferNumber, 1, &m_pPixelBuffer);

	return true;
}

void FontShaderClass::RenderShader(ID3D11DeviceContext* immeidateContext, int indexCount)
{
	immeidateContext->IASetInputLayout(m_pInputLayout);
	

	immeidateContext->VSSetShader(m_pVertexShader, NULL, 0);
	immeidateContext->PSSetShader(m_pPixelShader, NULL, 0);

	immeidateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	immeidateContext->DrawIndexed(indexCount, 0, 0);
}

#include "stdafx.h"
#include "bumpMapShaderClass.h"

BumpMapShaderClass::BumpMapShaderClass()
{
}

BumpMapShaderClass::BumpMapShaderClass(const BumpMapShaderClass& other)
{
}

BumpMapShaderClass::~BumpMapShaderClass()
{
}

bool BumpMapShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	if (!InitializeShader(device, hwnd, L"Shaders/bumpMap.vs", L"Shaders/bumpMap.ps"))
	{
		return false;
	}

	return true;
}

void BumpMapShaderClass::Shutdown()
{
	ShutdownShader();
}

bool BumpMapShaderClass::Render(ID3D11DeviceContext* immediateContext, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView** textures, XMFLOAT3 lightDir, XMFLOAT4 diffuseColor)
{
	if (!SetShaderParameters(immediateContext, world, view, proj, textures, lightDir, diffuseColor))
	{
		return false;
	}

	RenderShader(immediateContext, indexCount);

	return true;
}

bool BumpMapShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ID3D10Blob* pErrorMsg = nullptr;

	ID3D10Blob* pVertexBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "BumpMapVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pVertexBuffer, &pErrorMsg)))
	{
		if (pErrorMsg)
		{
			OutputShaderErrorMessage(pErrorMsg, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing VertexShader File", MB_OK);
		}

		return false;
	}

	ID3D10Blob* pPixelBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "BumpMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pPixelBuffer, &pErrorMsg)))
	{
		if (pErrorMsg)
		{
			OutputShaderErrorMessage(pErrorMsg, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing PixelShader File", MB_OK);
		}

		return false;
	}

	if (FAILED(device->CreateVertexShader(pVertexBuffer->GetBufferPointer(), pVertexBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		return false;
	}

	if (FAILED(device->CreatePixelShader(pPixelBuffer->GetBufferPointer(), pPixelBuffer->GetBufferSize(), NULL, &m_pPixelShader)))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[5] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(device->CreateInputLayout(polygonLayout, _countof(polygonLayout), pVertexBuffer->GetBufferPointer(), pVertexBuffer->GetBufferSize(), &m_pInputLayout)))
	{
		return false;
	}

	SAFE_RELEASE(pPixelBuffer);
	SAFE_RELEASE(pVertexBuffer);

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer)))
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;
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

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&lightBufferDesc, NULL, &m_pLightBuffer)))
	{
		return false;
	}

	return true;
}

void BumpMapShaderClass::ShutdownShader()
{
	SAFE_RELEASE(m_pLightBuffer);
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pMatrixBuffer);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexShader);
}

void BumpMapShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilename)
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

bool BumpMapShaderClass::SetShaderParameters(ID3D11DeviceContext* immediateContext, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView** textures, XMFLOAT3 lightDir, XMFLOAT4 diffuseColor)
{
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	proj = XMMatrixTranspose(proj);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));

	if (FAILED(immediateContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	MatrixBufferType* pMatrixBuffer = (MatrixBufferType*)mappedResource.pData;

	pMatrixBuffer->world = world;
	pMatrixBuffer->view = view;
	pMatrixBuffer->projection = proj;

	immediateContext->Unmap(m_pMatrixBuffer, NULL);

	immediateContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	immediateContext->PSSetShaderResources(0, 2, textures);

	if (FAILED(immediateContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	LightBufferType* pLightBuffer = (LightBufferType*)mappedResource.pData;

	pLightBuffer->diffuseColor = diffuseColor;
	pLightBuffer->lightDirection = lightDir;
	pLightBuffer->padding = 1.0f;

	immediateContext->Unmap(m_pLightBuffer, NULL);

	immediateContext->PSSetConstantBuffers(0, 1, &m_pLightBuffer);

	return true;
}

void BumpMapShaderClass::RenderShader(ID3D11DeviceContext* immediateContext, int indexCount)
{
	immediateContext->IASetInputLayout(m_pInputLayout);

	immediateContext->VSSetShader(m_pVertexShader, NULL, NULL);
	immediateContext->PSSetShader(m_pPixelShader, NULL, NULL);

	immediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	immediateContext->DrawIndexed(indexCount, 0, 0);
}

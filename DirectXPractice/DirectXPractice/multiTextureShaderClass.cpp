#include "stdafx.h"
#include "multiTextureShaderClass.h"

MultiTextureShaderClass::MultiTextureShaderClass()
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pInputLayout = nullptr;
	m_pMatrixBuffer = nullptr;
	m_pSamplerState = nullptr;
}

MultiTextureShaderClass::MultiTextureShaderClass(const MultiTextureShaderClass&)
{
}

MultiTextureShaderClass::~MultiTextureShaderClass()
{
}

bool MultiTextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	if (!InitializeShader(device, hwnd, L"Shaders/multiTexture.vs", L"Shaders/multiTexture.ps"))
	{
		return false;
	}

	return true;
}

void MultiTextureShaderClass::Shutdown()
{
	ShutdownShader();
}

bool MultiTextureShaderClass::Render(ID3D11DeviceContext* immediateContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, ID3D11ShaderResourceView** textures)
{
	if (!SetShaderParameters(immediateContext, worldMatrix, viewMatrix, projMatrix, textures))
	{
		return false;
	}

	RenderShader(immediateContext, indexCount);

	return true;
}

bool MultiTextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ID3D10Blob* pErrorMsg = nullptr;

	ID3D10Blob* pVertexBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "MultiTextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pVertexBuffer, &pErrorMsg)))
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
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "MultiTexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pPixelBuffer, &pErrorMsg)))
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

	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	if (FAILED(device->CreateInputLayout(polygonLayout, _countof(polygonLayout), pVertexBuffer->GetBufferPointer(), pVertexBuffer->GetBufferSize(), &m_pInputLayout)))
	{
		return false;
	}

	SAFE_RELEASE(pPixelBuffer);
	SAFE_RELEASE(pVertexBuffer);

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(MatrixBufferType);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&bufferDesc, NULL, &m_pMatrixBuffer)))
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

	return true;
}

void MultiTextureShaderClass::ShutdownShader()
{
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pMatrixBuffer);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexShader);
}

void MultiTextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilename)
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

bool MultiTextureShaderClass::SetShaderParameters(ID3D11DeviceContext* immediateContext, XMMATRIX worldMatrix, XMMATRIX viewMatirx, XMMATRIX projMatrix, ID3D11ShaderResourceView** textures)
{
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatirx = XMMatrixTranspose(viewMatirx);
	projMatrix = XMMatrixTranspose(projMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));

	if (FAILED(immediateContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	MatrixBufferType* pMatrixData = (MatrixBufferType*)mappedResource.pData;

	pMatrixData->worldMatrix = worldMatrix;
	pMatrixData->viewMatrix = viewMatirx;
	pMatrixData->projectionMatrix = projMatrix;

	immediateContext->Unmap(m_pMatrixBuffer, 0);

	UINT bufferNumber = 0;

	immediateContext->VSSetConstantBuffers(bufferNumber, 1, &m_pMatrixBuffer);
	immediateContext->PSSetShaderResources(0, 2, textures);

	return true;
}

void MultiTextureShaderClass::RenderShader(ID3D11DeviceContext* immediateContext, int indexCount)
{
	immediateContext->IASetInputLayout(m_pInputLayout);

	immediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	immediateContext->PSSetShader(m_pPixelShader, NULL, 0);

	immediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	immediateContext->DrawIndexed(indexCount, 0, 0);
}

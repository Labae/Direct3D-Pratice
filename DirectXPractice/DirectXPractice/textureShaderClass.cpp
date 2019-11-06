#include "stdafx.h"
#include "textureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pInputLayout = nullptr;
	m_pMatrixBuffer = nullptr;
	m_pSamplerState = nullptr;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass&)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	if (!InitializeShader(device, hwnd, L"Shaders/texture.vs", L"Shaders/texture.ps"))
	{
		return false;
	}

	return true;
}

void TextureShaderClass::Shutdown()
{
	ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext* immediateContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, ID3D11ShaderResourceView* texture)
{
	if (!SetShaderParameters(immediateContext, worldMatrix, viewMatrix, projMatrix, texture))
	{
		return false;
	}

	RenderShader(immediateContext, indexCount);

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ID3D10Blob* pErrorMsg = nullptr;

	ID3D10Blob* pVertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pVertexShaderBuffer, &pErrorMsg)))
	{
		if (pErrorMsg)
		{
			OutputShaderErrorMessage(pErrorMsg, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing VS Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 셰이더 코드 컴파일.
	ID3D10Blob* pPixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pPixelShaderBuffer, &pErrorMsg)))
	{
		if (pErrorMsg)
		{
			OutputShaderErrorMessage(pErrorMsg, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing PS Shader File", MB_OK);
		}

		return false;
	}


	// 버퍼 정점 셰이더 생성.
	if (FAILED(device->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		return false;
	}

	// 버퍼 픽셀 셰이더 생성.
	if (FAILED(device->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader)))
	{
		return false;
	}

	// Input Layout 구조체 설정.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// Input Layout 구조체 생성.
	if (FAILED(device->CreateInputLayout(polygonLayout, _countof(polygonLayout), pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &m_pInputLayout)))
	{
		return false;
	}

	SAFE_RELEASE(pPixelShaderBuffer);
	SAFE_RELEASE(pVertexShaderBuffer);

	// 정점 셰이더에 있는 행렬 상수 버퍼를 작성.
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 생성.
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer)))
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

void TextureShaderClass::ShutdownShader()
{
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pMatrixBuffer);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexShader);
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilename)
{
	// 에러 메세지 출력창에 표시.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMsg->GetBufferPointer()));

	SAFE_RELEASE(errorMsg);

	MessageBox(hwnd, L"Error Compiling shader.", shaderFilename, MB_OK);
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* immediateContext, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView* texture)
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

	immediateContext->Unmap(m_pMatrixBuffer, 0);

	UINT iBufferNumber = 0;

	immediateContext->VSSetConstantBuffers(iBufferNumber, 1, &m_pMatrixBuffer);
	immediateContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* immediateContext, int indexCount)
{
	immediateContext->IASetInputLayout(m_pInputLayout);

	immediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	immediateContext->PSSetShader(m_pPixelShader, NULL, 0);

	immediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	immediateContext->DrawIndexed(indexCount, 0, 0);
}

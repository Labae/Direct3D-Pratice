#include "stdafx.h"
#include "lightShaderClass.h"

LightShaderClass::LightShaderClass()
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pInputLayout = nullptr;
	m_pSamplerState = nullptr;
	m_pMatrixBuffer = nullptr;
	m_pLightBuffer = nullptr;
	m_pCameraBuffer = nullptr;
}

LightShaderClass::LightShaderClass(const LightShaderClass&)
{
}

LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	return InitializeShader(device, hwnd, L"Shaders/light.vs", L"Shaders/light.ps");
}

void LightShaderClass::Shutdown()
{
	ShutdownShader();
}

bool LightShaderClass::Render(ID3D11DeviceContext* immediateContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 ambientColor, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lightDir, float specularPower, XMFLOAT4 specularColor, XMFLOAT3 cameraPosition)
{
	if (!SetShaderParameters(immediateContext, worldMatrix, viewMatrix, projMatrix, texture, ambientColor, lightDiffuseColor, lightDir, specularPower, specularColor, cameraPosition))
	{
		return false;
	}

	RenderShader(immediateContext, indexCount);

	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ID3D10Blob* pErrorMsg = nullptr;

	ID3D10Blob* pVertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pVertexShaderBuffer, &pErrorMsg)))
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
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pPixelShaderBuffer, &pErrorMsg)))
	{
		if (pErrorMsg)
		{
			OutputShaderErrorMessage(pErrorMsg, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Pixel Shader File", MB_OK);
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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	if (FAILED(device->CreateInputLayout(inputElementDesc, _countof(inputElementDesc), pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &m_pInputLayout)))
	{
		return false;
	}

	SAFE_RELEASE(pPixelShaderBuffer);
	SAFE_RELEASE(pVertexShaderBuffer);

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

	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
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

	D3D11_BUFFER_DESC cameraBufferDesc;
	ZeroMemory(&cameraBufferDesc, sizeof(cameraBufferDesc));
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&cameraBufferDesc, NULL, &m_pCameraBuffer)))
	{
		return false;
	}

	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(lightBufferDesc));
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

void LightShaderClass::ShutdownShader()
{
	SAFE_RELEASE(m_pCameraBuffer);
	SAFE_RELEASE(m_pLightBuffer);
	SAFE_RELEASE(m_pMatrixBuffer);
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexShader);
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilename)
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

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* immediateContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 ambientColor, XMFLOAT4 lightDiffuseColor, XMFLOAT3 lightDir, float specularPower, XMFLOAT4 specularColor, XMFLOAT3 cameraPosition)
{
	UINT iBufferNumber;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projMatrix = XMMatrixTranspose(projMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));
	if (FAILED(immediateContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	MatrixBufferType* pMatrixBuffer = (MatrixBufferType*)mappedResource.pData;
	iBufferNumber = 0;

	pMatrixBuffer->world = worldMatrix;
	pMatrixBuffer->view = viewMatrix;
	pMatrixBuffer->projection = projMatrix;

	immediateContext->Unmap(m_pMatrixBuffer, 0);

	immediateContext->VSSetConstantBuffers(iBufferNumber, 1, &m_pMatrixBuffer);

	if (FAILED(immediateContext->Map(m_pCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	CameraBufferType* pCameraBuffer = (CameraBufferType*)mappedResource.pData;
	iBufferNumber = 1;

	pCameraBuffer->cameraPosition = cameraPosition;
	pCameraBuffer->padding = 0.0f;

	immediateContext->Unmap(m_pCameraBuffer, 0);

	immediateContext->VSSetConstantBuffers(iBufferNumber, 1, &m_pCameraBuffer);
	immediateContext->PSSetShaderResources(0, 1, &texture);

	if (FAILED(immediateContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	LightBufferType* pLightBuffer = (LightBufferType*)mappedResource.pData;
	iBufferNumber = 0;

	pLightBuffer->ambientColor = ambientColor;
	pLightBuffer->diffuseColor = lightDiffuseColor;
	pLightBuffer->lightDirection = lightDir;
	pLightBuffer->specularPower = specularPower;
	pLightBuffer->specularColor = specularColor;

	immediateContext->Unmap(m_pLightBuffer, 0);

	immediateContext->PSSetConstantBuffers(iBufferNumber, 1, &m_pLightBuffer);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* immediateContext, int indexCount)
{
	immediateContext->IASetInputLayout(m_pInputLayout);

	immediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	immediateContext->PSSetShader(m_pPixelShader, NULL, 0);

	immediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	immediateContext->DrawIndexed(indexCount, 0, 0);
}

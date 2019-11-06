#include "stdafx.h"
#include "colorShaderClass.h"

ColorShaderClass::ColorShaderClass()
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pInputLayout = nullptr;
	m_pMatrixBuffer = nullptr;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass&)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	return InitializeShader(device, hwnd, L"../DirectXPractice/color.vs", L"../DirectXPractice/color.ps");
}

void ColorShaderClass::Shutdown()
{
	ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* immediateContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix)
{
	if (!SetShaderParameter(immediateContext, worldMatrix, viewMatrix, projMatrix))
	{
		return false;
	}

	RenderShader(immediateContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ID3D10Blob* pErrorMsg = nullptr;

	// ���� ���̴� �ڵ� ������.
	ID3D10Blob* pVertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pVertexShaderBuffer, &pErrorMsg)))
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

	// �ȼ� ���̴� �ڵ� ������.
	ID3D10Blob* pPixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pPixelShaderBuffer, &pErrorMsg)))
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

	// ���۷κ��� ���� ���̴� ����.
	if (FAILED(device->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		return false;
	}
	// ���۷κ��� �ȼ� ���̴� ����.
	if (FAILED(device->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader)))
	{
		return false;
	}

	// ���� ���̾ƿ� ����ü ����.
	// �� ������ ModelClass�� ���̴��� VertexType ������ ��ġ�ؾ���.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// ���� ���̾ƿ� ����ü ����.
	if (FAILED(device->CreateInputLayout(polygonLayout, _countof(polygonLayout), pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &m_pInputLayout)))
	{
		return false;
	}

	// ���� ���̴��� �ȼ� ���̴� ����.
	SAFE_RELEASE(pVertexShaderBuffer);
	SAFE_RELEASE(pPixelShaderBuffer);

	// ��� ��� ���� ����ü �ۼ�.
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� ������ ����.
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer)))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	SAFE_RELEASE(m_pMatrixBuffer);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexShader);
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilename)
{
	OutputDebugStringA(reinterpret_cast<const char*>(errorMsg->GetBufferPointer()));
	SAFE_RELEASE(errorMsg);

	MessageBox(hwnd, L"Error Compile Shader.", shaderFilename, MB_OK);
}

bool ColorShaderClass::SetShaderParameter(ID3D11DeviceContext* immediateContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix)
{
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projMatrix = XMMatrixTranspose(projMatrix);

	// ��� ���ۿ� �� ������ ���.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));
	if (FAILED(immediateContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� ������.
	MatrixBufferType* pData = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� ����.
	pData->world = worldMatrix;
	pData->view = viewMatrix;
	pData->projection = projMatrix;

	// ��� ������ ��� ����.
	immediateContext->Unmap(m_pMatrixBuffer, 0);

	// ���� ���̴����� ��� ���� ��ġ�� ����.
	unsigned int iBufferNumber = 0;

	// ���� ���̴��� ��� ���۸� �ٲ� ������ ����.
	immediateContext->VSSetConstantBuffers(iBufferNumber, 1, &m_pMatrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* immediateContext, int indexCount)
{
	// ���� �Է� ���̾ƿ� ����.
	immediateContext->IASetInputLayout(m_pInputLayout);

	// ���� ���̴� ����.
	immediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	// �ȼ� ���̴� ����.
	immediateContext->PSSetShader(m_pPixelShader, NULL, 0);

	// �ﰢ�� �׸���.
	immediateContext->DrawIndexed(indexCount, 0, 0);
}

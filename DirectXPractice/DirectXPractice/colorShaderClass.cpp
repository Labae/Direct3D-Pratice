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

	// 정점 셰이더 코드 컴파일.
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

	// 픽셀 셰이더 코드 컴파일.
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

	// 버퍼로부터 정점 셰이더 생성.
	if (FAILED(device->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		return false;
	}
	// 버퍼로부터 픽셀 셰이더 생성.
	if (FAILED(device->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader)))
	{
		return false;
	}

	// 정점 레이아웃 구조체 설정.
	// 이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야함.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// 정점 레이아웃 구조체 생성.
	if (FAILED(device->CreateInputLayout(polygonLayout, _countof(polygonLayout), pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &m_pInputLayout)))
	{
		return false;
	}

	// 정점 셰이더와 픽셀 셰이더 해제.
	SAFE_RELEASE(pVertexShaderBuffer);
	SAFE_RELEASE(pPixelShaderBuffer);

	// 행렬 상수 버퍼 구조체 작성.
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터 생성.
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

	// 상수 버퍼에 쓸 내용을 잠굼.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));
	if (FAILED(immediateContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옴.
	MatrixBufferType* pData = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사.
	pData->world = worldMatrix;
	pData->view = viewMatrix;
	pData->projection = projMatrix;

	// 상수 버퍼의 잠금 해제.
	immediateContext->Unmap(m_pMatrixBuffer, 0);

	// 정점 셰이더에서 상수 버퍼 위치를 설정.
	unsigned int iBufferNumber = 0;

	// 정점 셰이더의 상수 버퍼를 바뀐 값으로 설정.
	immediateContext->VSSetConstantBuffers(iBufferNumber, 1, &m_pMatrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* immediateContext, int indexCount)
{
	// 정점 입력 레이아웃 설정.
	immediateContext->IASetInputLayout(m_pInputLayout);

	// 정점 셰이더 설정.
	immediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	// 픽셀 셰이더 설정.
	immediateContext->PSSetShader(m_pPixelShader, NULL, 0);

	// 삼각형 그리기.
	immediateContext->DrawIndexed(indexCount, 0, 0);
}

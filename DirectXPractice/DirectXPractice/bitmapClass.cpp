#include "stdafx.h"
#include "textureClass.h"
#include "bitmapClass.h"

BitmapClass::BitmapClass()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_iVertexCount = 0;
	m_iIndexCount = 0;
	m_pTexture = nullptr;
}

BitmapClass::BitmapClass(const BitmapClass&)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	m_iScreenWidth = screenWidth;
	m_iScreenHeight = screenHeight;

	m_iBitmapWidth = bitmapWidth;
	m_iBitmapHeight = bitmapHeight;

	m_iPreviousPosX = -1;
	m_iPreviousPosY = -1;

	if (!InitializeBuffers(device))
	{
		return false;
	}

	if (!LoadTexture(device, textureFilename))
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

bool BitmapClass::Render(ID3D11DeviceContext* immediateContext, int positionX, int positionY)
{
	if (!UpdateBuffers(immediateContext, positionX, positionY))
	{
		return false;
	}

	RenderBuffers(immediateContext);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_iIndexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_pTexture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	m_iIndexCount = m_iVertexCount = 6;

	VertexType* pVertices = new VertexType[m_iVertexCount];
	if (!pVertices)
	{
		return false;
	}

	memset(pVertices, 0, (sizeof(VertexType) * m_iVertexCount));

	UINT* pIndices = new UINT[m_iIndexCount];
	if (!pIndices)
	{
		return false;
	}

	for (int i = 0; i < m_iIndexCount; i++)
	{
		pIndices[i] = i;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_iVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));

	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer)))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_iIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));

	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer)))
	{
		return false;
	}

	delete[] pIndices;
	pIndices = nullptr;

	delete[] pVertices;
	pVertices = nullptr;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* immediateContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* pVertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* pVerticesPtr;


	// 현재 올바른 매개 변수가 있음.
	if ((positionX == m_iPreviousPosX) && (positionY == m_iPreviousPosY))
	{
		return true;
	}


	// 렌더링 될 위치를 갱신.
	m_iPreviousPosX = positionX;
	m_iPreviousPosY = positionY;


	left = (float)((m_iScreenWidth / 2) * -1) + (float)positionX;
	right = left + (float)m_iBitmapWidth;
	top = (float)((m_iScreenHeight / 2)) - (float)positionY;
	bottom = top - (float)m_iBitmapHeight;

	pVertices = new VertexType[m_iVertexCount];
	if (!pVertices)
	{
		return false;
	}

	// 첫번째 삼각형.
	pVertices[0].position = XMFLOAT3(left, top, 0.0f);		// Top Left
	pVertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	pVertices[1].position = XMFLOAT3(right, bottom, 0.0f);	// Bottom Right
	pVertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	pVertices[2].position = XMFLOAT3(left, bottom, 0.0f);	// Bottom Left
	pVertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두번째 삼각형.
	pVertices[3].position = XMFLOAT3(left, top, 0.0f);		// Top Left
	pVertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	pVertices[4].position = XMFLOAT3(right, top, 0.0f);		// Top Right
	pVertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	pVertices[5].position = XMFLOAT3(right, bottom, 0.0f);	// Bottom Right
	pVertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	if (FAILED(immediateContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	pVerticesPtr = (VertexType*)mappedResource.pData;
	memcpy(pVerticesPtr, (void*)pVertices, (sizeof(VertexType) * m_iVertexCount));

	immediateContext->Unmap(m_pVertexBuffer, 0);

	delete[] pVertices;
	pVertices = nullptr;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* immediateContext)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	immediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* textureFilename)
{
	m_pTexture = new TextureClass;
	if (!m_pTexture)
	{
		return false;
	}

	return m_pTexture->Initialize(device, textureFilename);
}

void BitmapClass::ReleaseTexture()
{
	if (m_pTexture)
	{
		m_pTexture->Shutdown();
		delete m_pTexture;
		m_pTexture = nullptr;
	}
}

#include "stdafx.h"
#include "fontClass.h"
#include "fontShaderClass.h"
#include "textClass.h"

TextClass::TextClass()
{
	m_pFont = nullptr;
	m_pFontShader = nullptr;
	m_Sentence1 = nullptr;
	m_Sentence2 = nullptr;
	m_Sentence3 = nullptr;
}

TextClass::TextClass(const TextClass&)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* immediateContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatirx)
{
	m_iScreenWidth = screenWidth;
	m_iScreenHeight = screenHeight;

	m_BaseViewMatrix = baseViewMatirx;

	m_pFont = new FontClass;
	if (!m_pFont)
	{
		return false;
	}

	if (!m_pFont->Initialize(device, L"Datas/fontdata.txt", L"Textures/font.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the font object", L"Error", MB_OK);

		return false;
	}

	m_pFontShader = new FontShaderClass;
	if (!m_pFontShader)
	{
		return false;
	}

	if (!m_pFontShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the font shader object", L"Error", MB_OK);
		
		return false;
	}

	if (!InitializeSentence(&m_Sentence1, 16, device))
	{
		return false;
	}

	if (!InitializeSentence(&m_Sentence2, 16, device))
	{
		return false;
	}

	if (!InitializeSentence(&m_Sentence3, 32, device))
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	ReleaseText(&m_Sentence1);
	ReleaseText(&m_Sentence2);
	ReleaseText(&m_Sentence3);

	if (m_pFontShader)
	{
		m_pFontShader->Shutdown();
		delete m_pFontShader;
		m_pFontShader = nullptr;
	}

	if (m_pFont)
	{
		m_pFont->Shutdown();
		delete m_pFont;
		m_pFont = nullptr;
	}
}

bool TextClass::Render(ID3D11DeviceContext* immediateContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	if (!RenderSentence(immediateContext, m_Sentence1, worldMatrix, orthoMatrix))
	{
		return false;
	}

	if (!RenderSentence(immediateContext, m_Sentence2, worldMatrix, orthoMatrix))
	{
		return false;
	}

	if (!RenderSentence(immediateContext, m_Sentence3, worldMatrix, orthoMatrix))
	{
		return false;
	}

	return true;
}

bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* immediateContext)
{
	char tempString[16];
	char mouseString[16];
	bool result;

	_itoa_s(mouseX, tempString, 10);

	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	result = UpdateSentence(m_Sentence1, mouseString, 20, 20, 1.0f, 1.0f, 1.0f, immediateContext);
	if (!result)
	{
		return false;
	}

	_itoa_s(mouseY, tempString, 10);

	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	result = UpdateSentence(m_Sentence2, mouseString, 20, 40, 1.0f, 1.0f, 1.0f, immediateContext);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* immediateContext)
{
	if (fps > 9999)
	{
		fps = 9999;
	}

	// 정수를 문자열 형식으로 변환.
	char tempString[16] = { 0, };
	_itoa_s(fps, tempString, 10);

	char fpsString[16] = { 0, };
	strcpy_s(fpsString, "FPS : ");
	strcat_s(fpsString, tempString);

	float red = 0;
	float green = 0;
	float blue = 0;

	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	if (!UpdateSentence(m_Sentence1, fpsString, 20, 20, red, green, blue, immediateContext))
	{
		return false;
	}

	return true;
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* immediateContext)
{
	char tempString[16] = { 0, };
	_itoa_s(cpu, tempString, 10);

	char cpuString[16] = { 0, };
	strcpy_s(cpuString, "CPU : ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	if (!UpdateSentence(m_Sentence2, cpuString, 20, 40, 0.0f, 1.0f, 0.0f, immediateContext))
	{
		return false;
	}

	return true;
}

bool TextClass::SetRenderCount(int renderCount, ID3D11DeviceContext* immediateContext)
{
	char tempString[32] = { 0, };
	_itoa_s(renderCount, tempString, 10);

	char renderCountString[32] = { 0, };
	strcpy_s(renderCountString, "Render Count : ");
	strcat_s(renderCountString, tempString);

	if (!UpdateSentence(m_Sentence3, renderCountString, 20, 60, 0.0f, 1.0f, 0.0f, immediateContext))
	{
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{ 
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	(*sentence)->maxLength = maxLength;

	(*sentence)->vertexCount = 6 * maxLength;
	(*sentence)->indexCount = (*sentence)->vertexCount;

	VertexType* vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	UINT* indices = new UINT[(*sentence)->vertexCount];
	if (!indices)
	{
		return false;
	}

	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	for (int i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer)))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer)))
	{
		return false;
	}

	delete[] indices;
	indices = nullptr;

	delete[] vertices;
	vertices = nullptr;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green , float blue, ID3D11DeviceContext* immediateContext)
{
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	int iNumLetters = (int)strlen(text);

	if (iNumLetters > sentence->maxLength)
	{
		return false;
	}

	VertexType* vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	float drawX = (float)(((m_iScreenWidth / 2) * -1) + positionX);
	float drawY = (float)((m_iScreenHeight / 2) - positionY);

	m_pFont->BuildVertexArray((void*)vertices, text, drawX, drawY);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));

	if (FAILED(immediateContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	VertexType* verticesPtr = (VertexType*)mappedResource.pData;

	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	immediateContext->Unmap(sentence->vertexBuffer, 0);

	delete[] vertices;
	vertices = nullptr;

	return true;
}

void TextClass::ReleaseText(SentenceType** sentence)
{
	if (*sentence)
	{
		SAFE_RELEASE((*sentence)->vertexBuffer);
		SAFE_RELEASE((*sentence)->indexBuffer);

		delete* sentence;
		*sentence = nullptr;
	}
}

bool TextClass::RenderSentence(ID3D11DeviceContext* immediateContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	immediateContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	if (!m_pFontShader->Render(immediateContext, sentence->indexCount, worldMatrix, m_BaseViewMatrix, orthoMatrix, m_pFont->GetTexture(), pixelColor))
	{
		return false;
	}

	return true;
}

#include "stdafx.h"
#include "textureClass.h"
#include "fontClass.h"

FontClass::FontClass()
{
}

FontClass::FontClass(const FontClass&)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, WCHAR* fontFilename, WCHAR* textureFilename)
{
	if (!LoadFontData(fontFilename))
	{
		return false;
	}

	if(!LoadTexture(device, textureFilename))
	{
		return false;
	}

	return true;
}

void FontClass::Shutdown()
{
	ReleaseTexture();

	ReleaseFont();
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_pTexture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* pVertex;
	int iNumLetters, index, iLetter, i;

	pVertex = (VertexType*)vertices;
	iNumLetters = (int)strlen(sentence);

	index = 0;

	for (i = 0; i < iNumLetters; i++)
	{
		iLetter = ((int)sentence[i]) - 32;

		if (iLetter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// First Triangle in Quad
			pVertex[index].position = XMFLOAT3(drawX, drawY, 0.0f);
			pVertex[index].texture = XMFLOAT2(m_pFont[iLetter].left, 0.0f);
			index++;

			pVertex[index].position = XMFLOAT3((drawX + m_pFont[iLetter].size), (drawY - 16), 0.0f);
			pVertex[index].texture = XMFLOAT2(m_pFont[iLetter].right, 1.0f);
			index++;

			pVertex[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);
			pVertex[index].texture = XMFLOAT2(m_pFont[iLetter].left, 1.0f);
			index++;

			// Second Triangle in Quad
			pVertex[index].position = XMFLOAT3(drawX, drawY, 0.0f);
			pVertex[index].texture = XMFLOAT2(m_pFont[iLetter].left, 0.0f);
			index++;

			pVertex[index].position = XMFLOAT3((drawX + m_pFont[iLetter].size), drawY, 0.0f);
			pVertex[index].texture = XMFLOAT2(m_pFont[iLetter].right, 0.0f);
			index++;

			pVertex[index].position = XMFLOAT3((drawX + m_pFont[iLetter].size), (drawY - 16), 0.0f);
			pVertex[index].texture = XMFLOAT2(m_pFont[iLetter].right, 1.0f);
			index++;

			drawX = drawX + m_pFont[iLetter].size + 1.0f;
		}
	}
}

bool FontClass::LoadFontData(WCHAR* fontFilename)
{
	using namespace std;

	ifstream fin;
	int i;
	char temp;

	m_pFont = new FontType[95];
	if (!m_pFont)
	{
		return false;
	}

	fin.open(fontFilename);
	if (fin.fail())
	{
		return false;
	}

	for (i = 0; i < 95; i++)
	{
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_pFont[i].left;
		fin >> m_pFont[i].right;
		fin >> m_pFont[i].size;
	}

	fin.close();

	return true;
}

void FontClass::ReleaseFont()
{
	if (m_pFont)
	{
		delete[] m_pFont;
		m_pFont = nullptr;
	}
}

bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* textureFilename)
{
	m_pTexture = new TextureClass;
	if (!m_pTexture)
	{
		return false;
	}

	return m_pTexture->Initialize(device, textureFilename);
}

void FontClass::ReleaseTexture()
{
	if (m_pTexture)
	{
		m_pTexture->Shutdown();
		delete m_pTexture;
		m_pTexture = nullptr;
	}
}

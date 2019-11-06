#include "stdafx.h"
#include "textureArrayClass.h"

TextureArrayClass::TextureArrayClass()
{
	ZeroMemory(&m_pTextures, sizeof(m_pTextures));
}

TextureArrayClass::TextureArrayClass(const TextureArrayClass&)
{
}

TextureArrayClass::~TextureArrayClass()
{
}

bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2)
{
	ScratchImage image1 = LoadTextureFromFile(filename1);
	ScratchImage image2 = LoadTextureFromFile(filename2);

	if (FAILED(CreateShaderResourceView(device, image1.GetImages(), image1.GetImageCount(), image1.GetMetadata(), &m_pTextures[0])))
	{
		return false;
	}

	if (FAILED(CreateShaderResourceView(device, image2.GetImages(), image2.GetImageCount(), image2.GetMetadata(), &m_pTextures[1])))
	{
		return false;
	}

	return true;
}

void TextureArrayClass::Shutdown()
{
	SAFE_RELEASE(m_pTextures[0]);
	SAFE_RELEASE(m_pTextures[1]);
}

ScratchImage TextureArrayClass::LoadTextureFromFile(WCHAR* filename)
{
	HRESULT hr;
	std::wstring str(filename);
	std::wstring wsTmp(str.begin(), str.end());
	std::wstring ws = wsTmp;
	ScratchImage image;
	WCHAR ext[_MAX_EXT];
	_wsplitpath_s(ws.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	if (_wcsicmp(ext, L".dds") == 0)
	{
		hr = LoadFromDDSFile(ws.c_str(), DDS_FLAGS_NONE, nullptr, image);
	}
	else if (_wcsicmp(ext, L".tga") == 0)
	{
		hr = LoadFromTGAFile(ws.c_str(), nullptr, image);
	}
	else
	{
		hr = LoadFromWICFile(ws.c_str(), WIC_FLAGS_NONE, nullptr, image);
	}

	return image;
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_pTextures;
}

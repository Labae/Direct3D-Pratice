#include "stdafx.h"
#include "textureClass.h"

TextureClass::TextureClass()
{
	m_pTexture = nullptr;
}

TextureClass::TextureClass(const TextureClass&)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	ScratchImage image = LoadTextureFromFile(filename);
	if (FAILED(CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_pTexture)))
	{
		return false;
	}

	return true;
}

void TextureClass::Shutdown()
{
	SAFE_RELEASE(m_pTexture);
}

ScratchImage TextureClass::LoadTextureFromFile(WCHAR* filename)
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

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_pTexture;
}

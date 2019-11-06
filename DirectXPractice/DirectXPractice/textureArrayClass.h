#pragma once

#include "DirectXTex.h"
#include <string>

class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray();

private:
	ScratchImage LoadTextureFromFile(WCHAR*);

private:
	ID3D11ShaderResourceView* m_pTextures[2];
};


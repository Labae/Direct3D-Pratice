#pragma once

#include "DirectXTex.h"
#include <string>

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ScratchImage LoadTextureFromFile(WCHAR*);

private:
	ID3D11ShaderResourceView* m_pTexture;
};

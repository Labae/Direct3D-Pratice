#pragma once

class FontClass;
class FontShaderClass;

class TextClass : public AlignedAllocationPolicy<16>
{
private:
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX);
	bool SetMousePosition(int, int, ID3D11DeviceContext*);

	bool SetFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);
	
	bool SetRenderCount(int, ID3D11DeviceContext*);

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseText(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, XMMATRIX, XMMATRIX);

private:
	FontClass* m_pFont;
	FontShaderClass* m_pFontShader;
	int m_iScreenWidth;
	int m_iScreenHeight;
	XMMATRIX m_BaseViewMatrix;
	SentenceType* m_Sentence1;
	SentenceType* m_Sentence2;
	SentenceType* m_Sentence3;
};


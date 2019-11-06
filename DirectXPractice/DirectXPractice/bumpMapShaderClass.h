#pragma once
class BumpMapShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

public:
	BumpMapShaderClass();
	BumpMapShaderClass(const BumpMapShaderClass& other);
	~BumpMapShaderClass();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* immediateContext, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView** textures, XMFLOAT3 lightDir, XMFLOAT4 diffuseColor);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilename);

	bool SetShaderParameters(ID3D11DeviceContext* immediateContext, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView** textures, XMFLOAT3 lightDir, XMFLOAT4 diffuseColor);
	void RenderShader(ID3D11DeviceContext* immediateContext, int indexCount);

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pMatrixBuffer;
	ID3D11SamplerState* m_pSamplerState;
	ID3D11Buffer* m_pLightBuffer;
};


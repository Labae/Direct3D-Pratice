#pragma once


// Global
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class ModelClass;
//class ColorShaderClass;
//class TextureShaderClass;
class LightShaderClass;
class LightClass;
//class BitmapClass;
class TextClass;
class FrustumClass;
class ModelListClass;
class MultiTextureShaderClass;
class BumpMapShaderClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int, int, float, float);
	bool Render();

private:
	D3DClass* m_pDirect3D;
	CameraClass* m_pCamera;
	ModelClass* m_pModel;
	//ColorShaderClass* m_pColorShader;
	//TextureShaderClass* m_pTextureShader;
	//LightShaderClass* m_pLightShader;
	LightClass* m_pLight;
	//BitmapClass* m_pBitmap;
	TextClass* m_pText;
	FrustumClass* m_pFrustum;
	ModelListClass* m_pModelList;
	//MultiTextureShaderClass* m_pMultiTextureShader;
	BumpMapShaderClass* m_pBumpMapShader;
};


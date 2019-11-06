#include "stdafx.h"
#include "d3dClass.h"
#include "cameraClass.h"
#include "modelClass.h"
#include "lightClass.h"
//#include "lightShaderClass.h"
//#include "textureShaderClass.h"
//#include "colorShaderClass.h"
//#include "bitmapClass.h"
#include "textClass.h"
#include "frustumClass.h"
#include "modelListClass.h"
//#include "multiTextureShaderClass.h"
#include "graphicsClass.h"
#include "bumpMapShaderClass.h"

GraphicsClass::GraphicsClass()
{
	m_pDirect3D = nullptr;
	m_pCamera = nullptr;
	m_pModel = nullptr;
	//m_pColorShader = nullptr;
	//m_pTextureShader = nullptr;
	//m_pLightShader = nullptr;
	m_pLight = nullptr;
	//m_pBitmap = nullptr;
	m_pText = nullptr;
	m_pFrustum = nullptr;
	m_pModelList = nullptr;
	//m_pMultiTextureShader = nullptr;
	m_pBumpMapShader = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D 객체 생성.
	m_pDirect3D = new D3DClass;
	if (!m_pDirect3D)
	{
		return false;
	}

	// Direct3D 객체 초기화.
	if (!m_pDirect3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not Initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// 카메라 객체 생성.
	m_pCamera = new CameraClass;
	if (!m_pCamera)
	{
		return false;
	}

	XMMATRIX baseViewMatirx;
	// 카메라 객체 위치 설정.
	m_pCamera->SetPosition(0.0f, 0.0f, -1.0f);
	m_pCamera->Render();
	m_pCamera->GetViewMatrix(baseViewMatirx);

	// 모델 객체 생성.
	m_pModel = new ModelClass;
	if (!m_pModel)
	{
		return false;
	}

	// 모델 객체 초기화.
	if (!m_pModel->Initialize(m_pDirect3D->GetDevice(), L"Datas/Cube.txt", L"Textures/stone01.tga", L"Textures/bump01.dds"))
	{
		MessageBox(hwnd, L"Could not Initialize Model", L"Error", MB_OK);
		return false;
	}

	//m_pLightShader = new LightShaderClass;
	//if (!m_pLightShader)
	//{
	//	return false;
	//}

	//if (!m_pLightShader->Initialize(m_pDirect3D->GetDevice(), hwnd))
	//{
	//	MessageBox(hwnd, L"Could not Initialize Light Shader", L"Error", MB_OK);
	//	return false;
	//}

	m_pLight = new LightClass;
	if (!m_pLight)
	{
		return false;
	}

	m_pLight->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_pLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLight->SetDirection(0.0f, 0.0f, 1.0f);
	m_pLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLight->SetSpecularPower(100.0f);

	//m_pTextureShader = new TextureShaderClass;
	//if (!m_pTextureShader)
	//{
	//	return false;
	//}

	//if (!m_pTextureShader->Initialize(m_pDirect3D->GetDevice(), hwnd))
	//{
	//	MessageBox(hwnd, L"Could not Initialize Texture Shader", L"Error", MB_OK);
	//	return false;
	//}

	//// 색 셰이더 객체 생성.
	//m_pColorShader = new ColorShaderClass;
	//if (!m_pColorShader)
	//{
	//	return false;
	//}

	//// 모델 객체 초기화.
	//if (!m_pColorShader->Initialize(m_pDirect3D->GetDevice(), hwnd))
	//{
	//	MessageBox(hwnd, L"Could not Initialize Color Shader", L"Error", MB_OK);
	//	return false;
	//}

	//m_pBitmap = new BitmapClass;
	//if (!m_pBitmap)
	//{
	//	return false;
	//}

	//if (!m_pBitmap->Initialize(m_pDirect3D->GetDevice(), screenWidth, screenHeight, L"Textures/stone01.tga", 16, 16))
	//{
	//	MessageBox(hwnd, L"Could not Initialize Bitmap", L"Error", MB_OK);
	//	return false;
	//}

	m_pText = new TextClass;
	if (!m_pText)
	{
		return false;
	}

	if (!m_pText->Initialize(m_pDirect3D->GetDevice(), m_pDirect3D->GetImmediateContext(), hwnd, screenWidth, screenHeight, baseViewMatirx))
	{
		MessageBox(hwnd, L"Could not Initialize Text", L"Error", MB_OK);

		return false;
	}

	m_pFrustum = new FrustumClass;
	if (!m_pFrustum)
	{
		return false;
	}

	m_pModelList = new ModelListClass;
	if (!m_pModelList)
	{
		return false;
	}

	if (!m_pModelList->Initialize(25))
	{
		MessageBox(hwnd, L"Could not Initialize Model List", L"Error", MB_OK);
		return false;
	}

	m_pBumpMapShader = new BumpMapShaderClass;
	if (!m_pBumpMapShader)
	{
		return false;
	}

	if (!m_pBumpMapShader->Initialize(m_pDirect3D->GetDevice(), hwnd))
	{
		return false;
	}

	//m_pMultiTextureShader = new MultiTextureShaderClass;
	//if (!m_pMultiTextureShader)
	//{
	//	return false;
	//}

	//if (!m_pMultiTextureShader->Initialize(m_pDirect3D->GetDevice(), hwnd))
	//{
	//	MessageBox(hwnd, L"Could not Initialize MultiTexture Shader", L"Error", MB_OK);
	//	return false;
	//}

	return true;
}

void GraphicsClass::Shutdown()
{
	//if (m_pMultiTextureShader)
	//{
	//	m_pMultiTextureShader->Shutdown();
	//	delete m_pMultiTextureShader;
	//	m_pMultiTextureShader = nullptr;
	//}

	if (m_pBumpMapShader)
	{
		m_pBumpMapShader->Shutdown();
		delete m_pBumpMapShader;
		m_pBumpMapShader = nullptr;
	}

	if (m_pModelList)
	{
		m_pModelList->Shutdown();
		delete m_pModelList;
		m_pModelList = nullptr;
	}

	if (m_pFrustum)
	{
		delete m_pFrustum;
		m_pFrustum = nullptr;
	}

	if (m_pText)
	{
		m_pText->Shutdown();
		delete m_pText;
		m_pText = nullptr;
	}

	//if (m_pColorShader)
	//{
	//	m_pColorShader->Shutdown();
	//	delete m_pColorShader;
	//	m_pColorShader = nullptr;
	//}

	//if (m_pBitmap)
	//{
	//	m_pBitmap->Shutdown();
	//	delete m_pBitmap;
	//	m_pBitmap = nullptr;
	//}

	//if (m_pTextureShader)
	//{
	//	m_pTextureShader->Shutdown();
	//	delete m_pTextureShader;
	//	m_pTextureShader = nullptr;
	//}

	if (m_pLight)
	{
		delete m_pLight;
		m_pLight = nullptr;
	}

	//if (m_pLightShader)
	//{
	//	m_pLightShader->Shutdown();
	//	delete m_pLightShader;
	//	m_pLightShader = nullptr;
	//}

	if (m_pModel)
	{
		m_pModel->Shutdown();
		delete m_pModel;
		m_pModel = nullptr;
	}

	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	if (m_pDirect3D)
	{
		m_pDirect3D->Shutdown();
		delete m_pDirect3D;
		m_pDirect3D = nullptr;
	}
} 

bool GraphicsClass::Frame(int fps, int cpu, float rotationX, float rotationY)
{
	/*if (!m_pText->SetMousePosition(mouseX, mouseY, m_pDirect3D->GetImmediateContext()))
	{
		return false;
	}*/

	if (!m_pText->SetFps(fps, m_pDirect3D->GetImmediateContext()))
	{
		return false;
	}

	if (!m_pText->SetCpu(cpu, m_pDirect3D->GetImmediateContext()))
	{
		return false;
	}

	m_pCamera->SetPosition(0.0f, 0.0f, -10.0f);
	m_pCamera->SetRotation(rotationX, rotationY, 0.0f);

	return true;

}

bool GraphicsClass::Render()
{
	float positionX = 0.0f;
	float positionY = 0.0f;
	float positionZ = 0.0f;
	float radius = 1.0f;
	XMFLOAT4 color;

	// Scene을 그리기 위해서	버퍼를 지움.
	m_pDirect3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 위치에 따라 뷰 행렬 생성.
	m_pCamera->Render();

	// 카메라와 Direct3D 객체에서 월드, 뷰, 투영 행렬을 받아옴.
	XMMATRIX worldMatrix, viewMatirx, projMatrix, orthoMatrix;
	m_pDirect3D->GetWorldMatrix(worldMatrix);
	m_pCamera->GetViewMatrix(viewMatirx);
	m_pDirect3D->GetProjefctionMatrix(projMatrix);
	m_pDirect3D->GetOrthoMatrix(orthoMatrix);

	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.000025f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}


	// Construct Frustum
	m_pFrustum->ConstructFrustum(SCREEN_DEPTH, projMatrix, viewMatirx);

	int modelCount = m_pModelList->GetModelCount();

	int renderCount = 0;

	for (int index = 0; index < modelCount; index++)
	{
		m_pModelList->GetData(index, positionX, positionY, positionZ, color);
		m_pLight->SetDiffuseColor(color.x, color.y, color.z, color.w);

		if (m_pFrustum->CheckCube(positionX, positionY, positionZ, radius))
		{
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ) * XMMatrixRotationY(rotation);

			m_pModel->Render(m_pDirect3D->GetImmediateContext());

			//m_pLightShader->Render(m_pDirect3D->GetImmediateContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatirx, projMatrix, m_pModel->GetTexture(), m_pLight->GetAmbientColor(), m_pLight->GetDiffuseColor(), m_pLight->GetDirection(), m_pLight->GetSpecularPower(), m_pLight->GetSpecularColor(), m_pCamera->GetPosition());
		
			//m_pMultiTextureShader->Render(m_pDirect3D->GetImmediateContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatirx, projMatrix, m_pModel->GetTextureArray());

			m_pBumpMapShader->Render(m_pDirect3D->GetImmediateContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatirx, projMatrix, m_pModel->GetTextureArray(), m_pLight->GetDirection(), m_pLight->GetDiffuseColor());

			m_pDirect3D->GetWorldMatrix(worldMatrix);

			renderCount++;
		}
	}

	if (!m_pText->SetRenderCount(renderCount, m_pDirect3D->GetImmediateContext()))
	{
		return false;
	}
	
	m_pDirect3D->TurnZBufferOff();

	m_pDirect3D->TurnOnAlphaBlending();

	if (!m_pText->Render(m_pDirect3D->GetImmediateContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	m_pDirect3D->TurnOffAlphaBlending();

	m_pDirect3D->TurnZBufferOn();

	// 버퍼의 내용을 화면에 출력합니다.
	m_pDirect3D->EndScene();

	return true;
}

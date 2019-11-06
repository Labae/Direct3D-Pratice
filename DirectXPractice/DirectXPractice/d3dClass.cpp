#include "stdafx.h"
#include "d3dClass.h"

D3DClass::D3DClass()
{
	m_bVsync_enabled = false;
	m_iVideoCardMemory = 0;
	m_pSwapChain = nullptr;
	m_pDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDepthStencilBuffer = nullptr;
	m_pDepthStencilState = nullptr;
	m_pDepthStencilView = nullptr;
	m_pRasterState = nullptr;
	m_pDepthDisabledStencilState = nullptr;
	m_pAlphaEnableBlendingState = nullptr;
	m_pAlphaDisableBlendingState = nullptr;
}

D3DClass::D3DClass(const D3DClass&)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	// 수직동기화 상태를 저장.
	m_bVsync_enabled = vsync;

	// DirectX 그래픽 인터페이스 팩토리를 생성.
	IDXGIFactory* pFactory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)& pFactory)))
	{
		return false;
	}

	// 팩토리 객체를 사용해서 첫번째 그래픽카드 인터페이스 어뎁터를 생성.
	UINT iAdapter = 0;
	IDXGIAdapter* pAdapter = nullptr;
	if (FAILED(pFactory->EnumAdapters(iAdapter, &pAdapter)))
	{
		return false;
	}

	// 출력(모니터)에 대한 첫번째 어뎁터를 지정.
	UINT iAdapterOutput = 0;
	IDXGIOutput* pAdapterOutput = nullptr;
	if (FAILED(pAdapter->EnumOutputs(iAdapterOutput, &pAdapterOutput)))
	{
		return false;
	}

	// 출력(모니터)에 대한 DXGI_FORMAT_R8G8B8A8_UNORM 표시 형식에 맞는 모드 수를 가져옴.
	unsigned int iNumModes = 0;
	if (FAILED(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &iNumModes, NULL)))
	{
		return false;
	}

	// 가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트를 생성.
	DXGI_MODE_DESC* pDisplayModeList = new DXGI_MODE_DESC[iNumModes];
	if (!pDisplayModeList)
	{
		return false;
	}

	// 이제 디스플레이 모드에 대한 리스트를 채워넣음.
	if (FAILED(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &iNumModes, pDisplayModeList)))
	{
		return false;
	}

	// 이제 모든 디스플레이 모드에 대한 리스트를 채움.
	// 적합한 것을 찾으면 모니터의 새로고침 비율의 분모와 분자 값을 저장.
	unsigned int iNumerator = 0;
	unsigned int iDenominator = 0;
	unsigned int i = 0;
	for (i = 0; i < iNumModes; i++)
	{
		if (pDisplayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (pDisplayModeList[i].Height == (unsigned int)screenHeight)
			{
				iNumerator = pDisplayModeList[i].RefreshRate.Numerator;
				iDenominator = pDisplayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// 비디오카드 구조체를 가져옴.
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(pAdapter->GetDesc(&adapterDesc)))
	{
		return false;
	}

	// 비디오카드 메모리 용량 단위를 메가바이트 단위로 저장.
	m_iVideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 비디오카드 이름을 저장.
	size_t stringLength = 0;
	if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)
	{
		return false;
	}

	// 디스플레이 모드 리스트를 해제.
	delete[] pDisplayModeList;
	pDisplayModeList = nullptr;

	// 출력 어뎁터 해제.
	pAdapterOutput->Release();
	pAdapterOutput = nullptr;

	// 어뎁터 해제.
	pAdapter->Release();
	pAdapter = nullptr;

	// 팩토리 객체 해제.
	pFactory->Release();
	pFactory = nullptr;

	// 스왑체인 객체를 초기화.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 백버퍼를 1개만 사용하도록 지정.
	swapChainDesc.BufferCount = 1;

	// 백버퍼의 넓이와 높이를 지정.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 32Bit 서페이스를 설정.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 백버퍼의 새로고침 비율을 설정.
	if (m_bVsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = iNumerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = iDenominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 백버퍼의 사용용도를 설정.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링에 사용될 윈도우 핸들을 지정.
	swapChainDesc.OutputWindow = hwnd;

	// 멀티 샘플링을 끔.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 윈도우모드나 풀스크린모드를 설정.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// 스캔 라인 순서 및 크기를 지정하지 않음으로 설정.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 출력한 후 백버퍼를 비우도록 설정.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 전체화면 전환 허용.
	swapChainDesc.Flags = 0;

	// 피러레벨을 DirectX 11로 설정.
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 스왑체인, 디바이스, 디바이스 컨텍스트를 생성.
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL,&m_pImmediateContext)))
	{
		return false;
	}

	// 백버퍼의 포인터를 받아옴.
	ID3D11Texture2D* pBackBuffer = nullptr;
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
	{
		return false;
	}

	// 백버퍼의 포인터로 렌더 타겟 뷰를 생성.
	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView)))
	{
		return false;
	}

	// 백버퍼의 포인터를 해제.
	pBackBuffer->Release();
	pBackBuffer = nullptr;

	// 깊이 버퍼 구조체 초기화.
	D3D11_TEXTURE2D_DESC depthBufferDesc;

	// 깊이 버퍼 구조체 작성.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 설정된 깊이버퍼 구조체를 사용하여 깊이 버퍼 텍스처를 생성.
	if (FAILED(m_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer)))
	{
		return false;
	}

	// 스탠실 상태 구조체를 초기화.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 스탠실 상태 구조체 작성.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.StencilReadMask = 0xFF;

	// 픽셀 정면의 스텐실 설정.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 픽셀 뒷면의 스텐실 설정.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이 스텐실 상태를 생성.
	if (FAILED(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState)))
	{
		return false;
	}

	// 깊이 스텐실 상태를 설정.
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	// 깊이 스텐실 뷰 구조체를 초기화.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이 스텐실 뷰 구조체 작성.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰를 생성.
	if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView)))
	{
		return false;
	}

	// 렌더링 대상 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프 라인에 바인드함.
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// 그려지는 폴리곤과 방법을 결정할 레스터 구조체를 설정.
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 방금 작성한 구조체에서 래스터 라이저 상태를 만듬.
	if (FAILED(m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState)))
	{
		return false;
	}

	// 이제 레스터 라이저 상태를 설정.
	m_pImmediateContext->RSSetState(m_pRasterState);

	// 뷰포트를 설정.
	D3D11_VIEWPORT viewPort;
	viewPort.Width = (float)screenWidth;
	viewPort.Height = (float)screenHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;

	// 뷰포트 생성.
	m_pImmediateContext->RSSetViewports(1, &viewPort);

	// 투영 행렬 생성.
	float fieldOfView = 3.141592654f / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D 렌더링을 위해 투영 행렬 생성.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// 세계 행렬을 항등 행렬로 초기화.
	m_worldMatrix = XMMatrixIdentity();

	// 2D 렌더링을 위한 직교 투영 행렬을 만듬.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;

	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(m_pDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_pDepthDisabledStencilState)))
	{
		return false;
	}

	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));

	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_DEST_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	if (FAILED(m_pDevice->CreateBlendState(&blendStateDesc, &m_pAlphaEnableBlendingState)))
	{
		return false;
	}

	blendStateDesc.RenderTarget[0].BlendEnable = FALSE;

	if (FAILED(m_pDevice->CreateBlendState(&blendStateDesc, &m_pAlphaDisableBlendingState)))
	{
		return false;
	}

	return true;
}

void D3DClass::Shutdown()
{
	// 종료 전 윈도우 모드로 설정하지 않으면 스왑 체인 해제할 때 예외가 발생함.
	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(false, NULL);
	}

	SAFE_RELEASE(m_pAlphaDisableBlendingState);
	SAFE_RELEASE(m_pAlphaEnableBlendingState);
	SAFE_RELEASE(m_pDepthDisabledStencilState);
	SAFE_RELEASE(m_pRasterState);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencilState);
	SAFE_RELEASE(m_pDepthStencilBuffer);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pImmediateContext);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pSwapChain);
}

void D3DClass::BeginScene(float r, float g, float b, float a)
{
	// 백버퍼를 지울 색을 지정.
	float color[4] = { r,g,b,a };

	// 백버퍼를 지움.
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, color);

	// 깊이 버퍼를 지움.
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	// 렌더링이 완료되었으므로 화면에 백버퍼를 표시.
	if (m_bVsync_enabled)
	{
		// 화면 새로고침 비율을 고정.
		m_pSwapChain->Present(1, 0);
	}
	else
	{
		// 가능한 빠르게 출력.
		m_pSwapChain->Present(0, 0);
	}
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_pDevice;
}

ID3D11DeviceContext* D3DClass::GetImmediateContext()
{
	return m_pImmediateContext;
}

void D3DClass::GetProjefctionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_iVideoCardMemory;
}

void D3DClass::TurnZBufferOn()
{
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
}

void D3DClass::TurnZBufferOff()
{
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthDisabledStencilState, 1);
}

void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4] = { 0.0f,0.0f,0.0f,0.0f };

	m_pImmediateContext->OMSetBlendState(m_pAlphaEnableBlendingState, blendFactor, 0xffffffff);
}

void D3DClass::TurnOffAlphaBlending()
{
	float blendFactor[4] = { 0.0f,0.0f,0.0f,0.0f };

	m_pImmediateContext->OMSetBlendState(m_pAlphaEnableBlendingState, blendFactor, 0xffffffff);
}

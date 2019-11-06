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
	// ��������ȭ ���¸� ����.
	m_bVsync_enabled = vsync;

	// DirectX �׷��� �������̽� ���丮�� ����.
	IDXGIFactory* pFactory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)& pFactory)))
	{
		return false;
	}

	// ���丮 ��ü�� ����ؼ� ù��° �׷���ī�� �������̽� ��͸� ����.
	UINT iAdapter = 0;
	IDXGIAdapter* pAdapter = nullptr;
	if (FAILED(pFactory->EnumAdapters(iAdapter, &pAdapter)))
	{
		return false;
	}

	// ���(�����)�� ���� ù��° ��͸� ����.
	UINT iAdapterOutput = 0;
	IDXGIOutput* pAdapterOutput = nullptr;
	if (FAILED(pAdapter->EnumOutputs(iAdapterOutput, &pAdapterOutput)))
	{
		return false;
	}

	// ���(�����)�� ���� DXGI_FORMAT_R8G8B8A8_UNORM ǥ�� ���Ŀ� �´� ��� ���� ������.
	unsigned int iNumModes = 0;
	if (FAILED(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &iNumModes, NULL)))
	{
		return false;
	}

	// ������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ�� ����.
	DXGI_MODE_DESC* pDisplayModeList = new DXGI_MODE_DESC[iNumModes];
	if (!pDisplayModeList)
	{
		return false;
	}

	// ���� ���÷��� ��忡 ���� ����Ʈ�� ä������.
	if (FAILED(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &iNumModes, pDisplayModeList)))
	{
		return false;
	}

	// ���� ��� ���÷��� ��忡 ���� ����Ʈ�� ä��.
	// ������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���� ���� ����.
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

	// ����ī�� ����ü�� ������.
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(pAdapter->GetDesc(&adapterDesc)))
	{
		return false;
	}

	// ����ī�� �޸� �뷮 ������ �ް�����Ʈ ������ ����.
	m_iVideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// ����ī�� �̸��� ����.
	size_t stringLength = 0;
	if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)
	{
		return false;
	}

	// ���÷��� ��� ����Ʈ�� ����.
	delete[] pDisplayModeList;
	pDisplayModeList = nullptr;

	// ��� ��� ����.
	pAdapterOutput->Release();
	pAdapterOutput = nullptr;

	// ��� ����.
	pAdapter->Release();
	pAdapter = nullptr;

	// ���丮 ��ü ����.
	pFactory->Release();
	pFactory = nullptr;

	// ����ü�� ��ü�� �ʱ�ȭ.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// ����۸� 1���� ����ϵ��� ����.
	swapChainDesc.BufferCount = 1;

	// ������� ���̿� ���̸� ����.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 32Bit �����̽��� ����.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ������� ���ΰ�ħ ������ ����.
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

	// ������� ���뵵�� ����.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� ���� ������ �ڵ��� ����.
	swapChainDesc.OutputWindow = hwnd;

	// ��Ƽ ���ø��� ��.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// �������峪 Ǯ��ũ����带 ����.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// ��ĵ ���� ���� �� ũ�⸦ �������� �������� ����.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ����� �� ����۸� ��쵵�� ����.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// ��üȭ�� ��ȯ ���.
	swapChainDesc.Flags = 0;

	// �Ƿ������� DirectX 11�� ����.
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// ����ü��, ����̽�, ����̽� ���ؽ�Ʈ�� ����.
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL,&m_pImmediateContext)))
	{
		return false;
	}

	// ������� �����͸� �޾ƿ�.
	ID3D11Texture2D* pBackBuffer = nullptr;
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
	{
		return false;
	}

	// ������� �����ͷ� ���� Ÿ�� �並 ����.
	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView)))
	{
		return false;
	}

	// ������� �����͸� ����.
	pBackBuffer->Release();
	pBackBuffer = nullptr;

	// ���� ���� ����ü �ʱ�ȭ.
	D3D11_TEXTURE2D_DESC depthBufferDesc;

	// ���� ���� ����ü �ۼ�.
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

	// ������ ���̹��� ����ü�� ����Ͽ� ���� ���� �ؽ�ó�� ����.
	if (FAILED(m_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer)))
	{
		return false;
	}

	// ���Ľ� ���� ����ü�� �ʱ�ȭ.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// ���Ľ� ���� ����ü �ۼ�.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.StencilReadMask = 0xFF;

	// �ȼ� ������ ���ٽ� ����.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �ȼ� �޸��� ���ٽ� ����.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ���� ���ٽ� ���¸� ����.
	if (FAILED(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState)))
	{
		return false;
	}

	// ���� ���ٽ� ���¸� ����.
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	// ���� ���ٽ� �� ����ü�� �ʱ�ȭ.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// ���� ���ٽ� �� ����ü �ۼ�.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ���� ���ٽ� �並 ����.
	if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView)))
	{
		return false;
	}

	// ������ ��� ��� ���� ���ٽ� ���۸� ��� ���� ������ ���ο� ���ε���.
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// �׷����� ������� ����� ������ ������ ����ü�� ����.
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

	// ��� �ۼ��� ����ü���� ������ ������ ���¸� ����.
	if (FAILED(m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState)))
	{
		return false;
	}

	// ���� ������ ������ ���¸� ����.
	m_pImmediateContext->RSSetState(m_pRasterState);

	// ����Ʈ�� ����.
	D3D11_VIEWPORT viewPort;
	viewPort.Width = (float)screenWidth;
	viewPort.Height = (float)screenHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;

	// ����Ʈ ����.
	m_pImmediateContext->RSSetViewports(1, &viewPort);

	// ���� ��� ����.
	float fieldOfView = 3.141592654f / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D �������� ���� ���� ��� ����.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// ���� ����� �׵� ��ķ� �ʱ�ȭ.
	m_worldMatrix = XMMatrixIdentity();

	// 2D �������� ���� ���� ���� ����� ����.
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
	// ���� �� ������ ���� �������� ������ ���� ü�� ������ �� ���ܰ� �߻���.
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
	// ����۸� ���� ���� ����.
	float color[4] = { r,g,b,a };

	// ����۸� ����.
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, color);

	// ���� ���۸� ����.
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	// �������� �Ϸ�Ǿ����Ƿ� ȭ�鿡 ����۸� ǥ��.
	if (m_bVsync_enabled)
	{
		// ȭ�� ���ΰ�ħ ������ ����.
		m_pSwapChain->Present(1, 0);
	}
	else
	{
		// ������ ������ ���.
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

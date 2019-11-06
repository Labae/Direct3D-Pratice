#pragma once

// Linking
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

// Includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "AlignedAllocationPolicy.h"

// Defines
#define SAFE_RELEASE(p) {if((p) != NULL) {(p)->Release(); (p) = NULL;}}
#define GET_FRAME_SPEED(speed) speed / 100000000.0f
//
// pch.h
// Header for standard system include files.
//

#pragma once

// Use the C++ standard templated min/max
#define NOMINMAX

#include <wrl.h>
#include <wrl/client.h>
#include <wincodec.h>
#include <concrt.h>
#include <ppltasks.h>
#include <winapifamily.h>

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <d3d11_3.h>
#include <dxgi1_4.h>

#include <pix.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <Windows.UI.Core.h>

//
//  DirectXMain.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GamePad.h"

#include <string>
#include <vector>

class Engine;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class DirectXMain : public DX::IDeviceNotify
{
public:
    DirectXMain();
    
	~DirectXMain();

	// Initialization and management
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	void Initialize(Engine* engine, HWND window, int width, int height);
#else
	void Initialize(Engine* engine, IUnknown* window, int width, int height, float dpi, DXGI_MODE_ROTATION rotation);
#endif
	
	void OnNewAudioDevice();

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	void OnWindowSizeChanged(int width, int height);
#else
	void OnWindowSizeChanged(int width, int height, float dpi, DXGI_MODE_ROTATION rotation);
	void ValidateDevice();
#endif
    
    // Properties
    void GetDefaultSize(int& width, int& height) const;

private:
	// Device resources.
	std::unique_ptr<DX::DeviceResources> _deviceResources;

	// Rendering loop timer.
	DX::StepTimer _timer;

	std::unique_ptr<DirectX::Keyboard> _keyboard;
	DirectX::Keyboard::KeyboardStateTracker _keys;
	std::unique_ptr<DirectX::Mouse> _mouse;
	std::unique_ptr<DirectX::GamePad> _gamePad;
	DirectX::GamePad::ButtonStateTracker _buttons[4];

	Engine* _engine;

	float _dpi;
	bool _isPointerPressed;
	bool _isDeviceLost;
	bool _isWindowsMobile;

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

	void beginPixEvent(PCWSTR pFormat, DX::DeviceResources* deviceResources = nullptr);
	void endPixEvent(DX::DeviceResources* deviceResources = nullptr);
};

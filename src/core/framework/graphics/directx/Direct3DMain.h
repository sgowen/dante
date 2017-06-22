//
//  Direct3DMain.h
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

class IEngine;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Direct3DMain : public DX::IDeviceNotify
{
public:
    Direct3DMain();
    
	~Direct3DMain();

	// Initialization and management
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	void Initialize(IEngine* engine, HWND window, int width, int height);
#else
	void Initialize(IEngine* engine, IUnknown* window, int width, int height, float dpi, DXGI_MODE_ROTATION rotation);
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
	std::unique_ptr<DX::DeviceResources> m_deviceResources;

	// Rendering loop timer.
	DX::StepTimer m_timer;

	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	DirectX::Keyboard::KeyboardStateTracker m_keys;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	std::unique_ptr<DirectX::GamePad> m_gamePad;
	DirectX::GamePad::ButtonStateTracker m_buttons;

	IEngine* m_engine;

	float m_fDPI;
	bool m_isPointerPressed;
	bool m_isDeviceLost;
	bool m_isWindowsMobile;

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

	void beginPixEvent(PCWSTR pFormat, DX::DeviceResources* deviceResources = nullptr);
	void endPixEvent(DX::DeviceResources* deviceResources = nullptr);
};

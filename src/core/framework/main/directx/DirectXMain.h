//
//  DirectXMain.h
//  noctisgames
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#pragma once

#include <framework/main/directx/DirectXDeviceResources.h>
#include <framework/main/directx/StepTimer.h>

#include "Keyboard.h"
#include "Mouse.h"
#include "GamePad.h"

#include <string>
#include <vector>

class EngineController;
class Engine;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class DirectXMain : public DX::IDeviceNotify
{
public:
	static void create();
    static DirectXMain* getInstance();
	static void destroy();

	// Entry point
	static int exec(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, EngineController* engineController);

	// Windows procedure
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// Initialization and management
    void Initialize(EngineController* engineController, HWND window, int width, int height);
	
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

    void OnWindowSizeChanged(int width, int height);
    
    // Properties
    void GetDefaultSize(int& width, int& height) const;

private:
	static DirectXMain* s_instance;

	// Device resources.
	std::unique_ptr<DX::DirectXDeviceResources> _deviceResources;

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

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

	void beginPixEvent(PCWSTR pFormat);
	void endPixEvent();

private:
	// ctor, copy ctor, and assignment should be private in a Singleton
	DirectXMain();
	~DirectXMain();
	DirectXMain(const DirectXMain&);
	DirectXMain& operator=(const DirectXMain&);
};

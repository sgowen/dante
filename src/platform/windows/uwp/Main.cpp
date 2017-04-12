//
// Main.cpp
//

#include "pch.h"
#include "Direct3DMain.h"
#include "ScreenInputManager.h"
#include "KeyboardInputManager.h"
#include "MainScreen.h"

#include <ppltasks.h>

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::Phone::UI::Input;
using namespace Windows::System::Profile;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::ViewManagement;
using namespace DirectX;

ref class ViewProvider sealed : public IFrameworkView
{
public:
    ViewProvider() :
		m_isPointerPressed(false),
        m_exit(false),
        m_visible(true),
        m_DPI(96.f),
        m_logicalWidth(800.f),
        m_logicalHeight(600.f),
        m_nativeOrientation(DisplayOrientations::None),
        m_currentOrientation(DisplayOrientations::None)
    {
    }

    // IFrameworkView methods
    virtual void Initialize(CoreApplicationView^ applicationView)
    {
        applicationView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &ViewProvider::OnActivated);

        CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &ViewProvider::OnSuspending);

        CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(this, &ViewProvider::OnResuming);

        m_main = std::make_unique<Direct3DMain>();
    }

    virtual void Uninitialize()
    {
        m_main.reset();
    }

    virtual void SetWindow(CoreWindow^ window)
    {
        window->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &ViewProvider::OnWindowSizeChanged);

        window->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &ViewProvider::OnVisibilityChanged);

        window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &ViewProvider::OnWindowClosed);

        auto dispatcher = CoreWindow::GetForCurrentThread()->Dispatcher;

        dispatcher->AcceleratorKeyActivated += ref new TypedEventHandler<CoreDispatcher^, AcceleratorKeyEventArgs^>(this, &ViewProvider::OnAcceleratorKeyActivated);

        auto currentDisplayInformation = DisplayInformation::GetForCurrentView();

        currentDisplayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &ViewProvider::OnDpiChanged);

        currentDisplayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &ViewProvider::OnOrientationChanged);

        DisplayInformation::DisplayContentsInvalidated += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &ViewProvider::OnDisplayContentsInvalidated);

		// Register for pointer events, which will be raised on the background thread.
		window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ViewProvider::OnPointerPressed);
		window->PointerMoved += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ViewProvider::OnPointerMoved);
		window->PointerReleased += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ViewProvider::OnPointerReleased);

		if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
		{
			HardwareButtons::BackPressed += ref new EventHandler<BackPressedEventArgs ^>(this, &ViewProvider::OnBackPressed);
		}

        m_DPI = currentDisplayInformation->LogicalDpi;

        m_logicalWidth = window->Bounds.Width;
        m_logicalHeight = window->Bounds.Height;

        m_nativeOrientation = currentDisplayInformation->NativeOrientation;
        m_currentOrientation = currentDisplayInformation->CurrentOrientation;

        int outputWidth = ConvertDipsToPixels(m_logicalWidth);
        int outputHeight = ConvertDipsToPixels(m_logicalHeight);

        DXGI_MODE_ROTATION rotation = ComputeDisplayRotation();

        if (rotation == DXGI_MODE_ROTATION_ROTATE90 || rotation == DXGI_MODE_ROTATION_ROTATE270)
        {
            std::swap(outputWidth, outputHeight);
        }

        m_main->Initialize(reinterpret_cast<IUnknown*>(window), outputWidth, outputHeight, m_DPI, rotation);

		Mouse::SetDpi(m_DPI);
    }

    virtual void Load(Platform::String^ entryPoint)
    {
    }

    virtual void Run()
    {
        while (!m_exit)
        {
            if (m_visible)
            {
                m_main->Tick();

                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
            }
            else
            {
                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
            }
        }
    }

protected:
    // Event handlers
    void OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
    {
        if (args->Kind == ActivationKind::Launch)
        {
            auto launchArgs = static_cast<LaunchActivatedEventArgs^>(args);

            if (launchArgs->PrelaunchActivated)
            {
                // Opt-out of Prelaunch
				Windows::ApplicationModel::Core::CoreApplication::Exit();
                return;
            }
        }

        int w, h;
        m_main->GetDefaultSize(w, h);

        m_DPI = DisplayInformation::GetForCurrentView()->LogicalDpi;

        ApplicationView::PreferredLaunchWindowingMode = ApplicationViewWindowingMode::FullScreen;

        auto desiredSize = Size(ConvertPixelsToDips(w), ConvertPixelsToDips(h));

        ApplicationView::PreferredLaunchViewSize = desiredSize;

        auto view = ApplicationView::GetForCurrentView();

        auto minSize = Size(ConvertPixelsToDips(320), ConvertPixelsToDips(200));

        view->SetPreferredMinSize(minSize);

        CoreWindow::GetForCurrentThread()->Activate();

        view->FullScreenSystemOverlayMode = FullScreenSystemOverlayMode::Minimal;

        view->TryResizeView(desiredSize);
    }

    void OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
    {
        auto deferral = args->SuspendingOperation->GetDeferral();

        create_task([this, deferral]()
        {
            m_main->OnSuspending();

            deferral->Complete();
        });
    }

    void OnResuming(Platform::Object^ sender, Platform::Object^ args)
    {
        m_main->OnResuming();
    }

    void OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
    {
        m_logicalWidth = sender->Bounds.Width;
        m_logicalHeight = sender->Bounds.Height;

        HandleWindowSizeChanged();
    }

    void OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
    {
        m_visible = args->Visible;
		if (m_visible)
		{
			m_main->OnActivated();
		}
		else
		{
			m_main->OnDeactivated();
		}
    }

    void OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
    {
        m_exit = true;
    }

    void OnAcceleratorKeyActivated(CoreDispatcher^, AcceleratorKeyEventArgs^ args)
    {
        if (args->EventType == CoreAcceleratorKeyEventType::SystemKeyDown
            && args->VirtualKey == VirtualKey::Enter
            && args->KeyStatus.IsMenuKeyDown
            && !args->KeyStatus.WasKeyDown)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            auto view = ApplicationView::GetForCurrentView();

			if (view->IsFullScreenMode)
			{
				view->ExitFullScreenMode();
			}
			else
			{
				view->TryEnterFullScreenMode();
			}

            args->Handled = true;
        }
    }

    void OnDpiChanged(DisplayInformation^ sender, Object^ args)
    {
        m_DPI = sender->LogicalDpi;

		Mouse::SetDpi(m_DPI);

        HandleWindowSizeChanged();
    }

    void OnOrientationChanged(DisplayInformation^ sender, Object^ args)
    {
        m_currentOrientation = sender->CurrentOrientation;

        HandleWindowSizeChanged();
    }

    void OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
    {
        m_main->ValidateDevice();
    }

	void OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ e)
	{
		float x = e->CurrentPoint->Position.X;
		float y = e->CurrentPoint->Position.Y;
		x = ConvertDipsToPixels(x);
		y = ConvertDipsToPixels(y);

		SCREEN_INPUT_MANAGER->onTouch(ScreenEventType_DOWN, x, y);

		m_isPointerPressed = true;
	}

	void OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ e)
	{
		if  (!m_isPointerPressed)
		{
			return;
		}
		
		float x = e->CurrentPoint->Position.X;
		float y = e->CurrentPoint->Position.Y;
		x = ConvertDipsToPixels(x);
		y = ConvertDipsToPixels(y);

		SCREEN_INPUT_MANAGER->onTouch(ScreenEventType_DRAGGED, x, y);
	}

	void OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ e)
	{
		float x = e->CurrentPoint->Position.X;
		float y = e->CurrentPoint->Position.Y;
		x = ConvertDipsToPixels(x);
		y = ConvertDipsToPixels(y);

		SCREEN_INPUT_MANAGER->onTouch(ScreenEventType_UP, x, y);
	}

	// Back Button Handling (only for Windows Phone)
	void OnBackPressed(Platform::Object^ sender, Windows::Phone::UI::Input::BackPressedEventArgs^ args)
	{
        KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_BACK, true);
        
        args->Handled = true;
	}

private:
	bool                    m_isPointerPressed;
    bool                    m_exit;
    bool                    m_visible;
    float                   m_DPI;
    float                   m_logicalWidth;
    float                   m_logicalHeight;
    std::unique_ptr<Direct3DMain>   m_main;

    Windows::Graphics::Display::DisplayOrientations	m_nativeOrientation;
    Windows::Graphics::Display::DisplayOrientations	m_currentOrientation;

    inline int ConvertDipsToPixels(float dips) const
    {
        return int(dips * m_DPI / 96.f + 0.5f);
    }

    inline float ConvertPixelsToDips(int pixels) const
    {
        return (float(pixels) * 96.f / m_DPI);
    }

    DXGI_MODE_ROTATION ComputeDisplayRotation() const
    {
        DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

        switch (m_nativeOrientation)
        {
		case DisplayOrientations::Landscape:
			switch (m_currentOrientation)
			{
			case DisplayOrientations::Landscape:
                rotation = DXGI_MODE_ROTATION_IDENTITY;
                break;

            case DisplayOrientations::Portrait:
                rotation = DXGI_MODE_ROTATION_ROTATE270;
                break;

            case DisplayOrientations::LandscapeFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE180;
                break;

            case DisplayOrientations::PortraitFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE90;
                break;
            }
            break;

        case DisplayOrientations::Portrait:
            switch (m_currentOrientation)
            {
            case DisplayOrientations::Landscape:
                rotation = DXGI_MODE_ROTATION_ROTATE90;
                break;

            case DisplayOrientations::Portrait:
                rotation = DXGI_MODE_ROTATION_IDENTITY;
                break;

            case DisplayOrientations::LandscapeFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE270;
                break;

            case DisplayOrientations::PortraitFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE180;
                break;
            }
            break;
        }

        return rotation;
    }

    void HandleWindowSizeChanged()
    {
        int outputWidth = ConvertDipsToPixels(m_logicalWidth);
        int outputHeight = ConvertDipsToPixels(m_logicalHeight);

        DXGI_MODE_ROTATION rotation = ComputeDisplayRotation();

        if (rotation == DXGI_MODE_ROTATION_ROTATE90 || rotation == DXGI_MODE_ROTATION_ROTATE270)
        {
            std::swap(outputWidth, outputHeight);
        }

        m_main->OnWindowSizeChanged(outputWidth, outputHeight, m_DPI, rotation);
    }
};

ref class ViewProviderFactory : IFrameworkViewSource
{
public:
    virtual IFrameworkView^ CreateView()
    {
        return ref new ViewProvider();
    }
};

// Entry point
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^ argv)
{
    UNREFERENCED_PARAMETER(argv);

    if (!XMVerifyCPUSupport())
    {
        throw std::exception("XMVerifyCPUSupport");
    }

    auto viewProviderFactory = ref new ViewProviderFactory();
    CoreApplication::Run(viewProviderFactory);
    return 0;
}

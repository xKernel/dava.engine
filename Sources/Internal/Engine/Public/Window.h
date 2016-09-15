#pragma once

#if defined(__DAVAENGINE_COREV2__)

#include "Base/BaseTypes.h"
#include "Functional/Signal.h"
#include "Math/Vector.h"

#include "UI/UIEvent.h"

#include "Engine/Private/EnginePrivateFwd.h"

namespace rhi
{
struct InitParam;
}

namespace DAVA
{
class InputSystem;
class UIControlSystem;
class VirtualCoordinatesSystem;

class Window final
{
public:
    bool IsPrimary() const;
    bool IsVisible() const;
    bool HasFocus() const;

    // Window size in logical pixels
    float32 GetWidth() const;
    float32 GetHeight() const;
    // Window's render surface size in pixels
    float32 GetRenderSurfaceWidth() const;
    float32 GetRenderSurfaceHeight() const;

    // Window scale factors
    float32 GetScaleX() const;
    float32 GetScaleY() const;
    // Additional user scale factor
    float32 GetUserScale() const;
    // Window's render surface scale factors
    float32 GetRenderSurfaceScaleX() const;
    float32 GetRenderSurfaceScaleY() const;

    Vector2 GetSize() const;
    Vector2 GetScale() const;

    void Resize(float32 w, float32 h);
    void Resize(Vector2 size);
    void Close();

    Engine* GetEngine() const;
    void* GetNativeHandle() const;
    WindowNativeService* GetNativeService() const;

    void RunAsyncOnUIThread(const Function<void()>& task);

    // Window's cursor mode
    void SetMouseMode(eMouseMode mode);
    eMouseMode GetMouseMode() const;

public:
    // For now these methods are public
    void PostFocusChanged(bool focus);
    void PostVisibilityChanged(bool visibility);
    void PostSizeChanged(float32 width, float32 height, float32 scaleX, float32 scaleY);
    void PostWindowCreated(Private::WindowBackend* wbackend, float32 width, float32 height, float32 scaleX, float32 scaleY);
    void PostWindowDestroyed();

    void PostKeyDown(uint32 key, bool isRepeated);
    void PostKeyUp(uint32 key);
    void PostKeyChar(uint32 key, bool isRepeated);

public:
    // Signals
    Signal<Window&, bool> visibilityChanged;
    Signal<Window&, bool> focusChanged;
    Signal<Window&> destroyed;
    Signal<Window&, float32, float32, float32, float32> sizeScaleChanged;
    //Signal<Window&> beginUpdate;
    //Signal<Window&> beginDraw;
    Signal<Window&, float32> update;
    //Signal<Window&> endDraw;
    //Signal<Window&> endUpdate;

private:
    Window(Private::EngineBackend* engine, bool primary);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

private:
    void Update(float32 frameDelta);
    void Draw();

    void EventHandler(const Private::MainDispatcherEvent& e);
    void FinishEventHandlingOnCurrentFrame();

    void HandleWindowCreated(const Private::MainDispatcherEvent& e);
    void HandleWindowDestroyed(const Private::MainDispatcherEvent& e);
    void HandleSizeChanged(const Private::MainDispatcherEvent& e);
    void HandleFocusChanged(const Private::MainDispatcherEvent& e);
    void HandleVisibilityChanged(const Private::MainDispatcherEvent& e);
    void HandleMouseClick(const Private::MainDispatcherEvent& e);
    void HandleMouseWheel(const Private::MainDispatcherEvent& e);
    void HandleMouseMove(const Private::MainDispatcherEvent& e);
    void HandleTouchClick(const Private::MainDispatcherEvent& e);
    void HandleTouchMove(const Private::MainDispatcherEvent& e);
    void HandleKeyPress(const Private::MainDispatcherEvent& e);
    void HandleKeyChar(const Private::MainDispatcherEvent& e);

    void HandlePendingSizeChanging();

    void ClearMouseButtons();
    void InitCustomRenderParams(rhi::InitParam& params);

private:
    Private::EngineBackend* engineBackend = nullptr;
    Private::MainDispatcher* dispatcher = nullptr;
    Private::WindowBackend* windowBackend = nullptr;

    InputSystem* inputSystem = nullptr;
    UIControlSystem* uiControlSystem = nullptr;
    VirtualCoordinatesSystem* virtualCoordSystem = nullptr;

    bool isPrimary = false;
    bool isVisible = false;
    bool hasFocus = false;
    float32 width = 0.0f;
    float32 height = 0.0f;
    float32 scaleX = 1.0f;
    float32 scaleY = 1.0f;
    float32 userScale = 1.0f;

    bool pendingInitRender = false;
    bool pendingSizeChanging = false;

    Bitset<static_cast<size_t>(UIEvent::MouseButton::NUM_BUTTONS)> mouseButtonState;

    eMouseMode mouseMode = eMouseMode::OFF;
    // Friends
    friend class Private::EngineBackend;
    friend Private::WindowBackend;
};

inline bool Window::IsPrimary() const
{
    return isPrimary;
}

inline bool Window::IsVisible() const
{
    return isVisible;
}

inline bool Window::HasFocus() const
{
    return hasFocus;
}

inline float32 Window::GetWidth() const
{
    return width;
}

inline float32 Window::GetHeight() const
{
    return height;
}

inline float32 Window::GetRenderSurfaceWidth() const
{
    return width * scaleX * userScale;
}

inline float32 Window::GetRenderSurfaceHeight() const
{
    return height * scaleY * userScale;
}

inline float32 Window::GetScaleX() const
{
    return scaleX;
}

inline float32 Window::GetScaleY() const
{
    return scaleY;
}

inline float32 Window::GetUserScale() const
{
    return userScale;
}

inline float32 Window::GetRenderSurfaceScaleX() const
{
    return scaleX * userScale;
}

inline float32 Window::GetRenderSurfaceScaleY() const
{
    return scaleY * userScale;
}

inline Vector2 Window::GetSize() const
{
    return Vector2(GetWidth(), GetHeight());
}

inline Vector2 Window::GetScale() const
{
    return Vector2(GetScaleX(), GetScaleY());
}

inline void Window::Resize(Vector2 size)
{
    Resize(size.dx, size.dy);
}

} // namespace DAVA

#endif // __DAVAENGINE_COREV2__

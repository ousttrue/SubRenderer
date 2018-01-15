#include "ImGuiRenderer.h"
#include <imgui.h>
#include <examples/directx11_example/imgui_impl_dx11.h>


bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_col = ImColor(114, 144, 154);

bool initialized = false;

static void Custom_ImGui_ImplDX11_NewFrame(int width, int height, float deltaSeconds)
{
    //if (!g_pFontSampler)

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2((float)width, (float)height);

    // Setup time step
    io.DeltaTime = deltaSeconds;

#ifdef UWP
#else
    // Read keyboard modifiers inputs
    io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
    io.KeySuper = false;
    // io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
    // io.MousePos : filled by WM_MOUSEMOVE events
    // io.MouseDown : filled by WM_*BUTTON* events
    // io.MouseWheel : filled by WM_MOUSEWHEEL events

    // Hide OS mouse cursor if ImGui is drawing it
    if (io.MouseDrawCursor)
        SetCursor(NULL);
#endif

    // Start the frame
    ImGui::NewFrame();
}

CImGuiRenderer::~CImGuiRenderer()
{
    ImGui_ImplDX11_Shutdown();
}

STDMETHODIMP CImGuiRenderer::CreateDeviceDependentResources(ID3D11Device *pDevice)
{
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    pDevice->GetImmediateContext(&pContext);
    ImGui_ImplDX11_Init(nullptr, pDevice, pContext.Get());

    return S_OK;
}

STDMETHODIMP CImGuiRenderer::SetTexture(ID3D11Texture2D *pTexture)
{
    return S_OK;
}

int m_width = 0;
int m_height = 0;
STDMETHODIMP CImGuiRenderer::SetSize(int width, int height)
{
    m_width = width;
    m_height = height;
    ImGui_ImplDX11_InvalidateDeviceObjects();
    initialized = false;

    return S_OK;
}

float m_now = 0;
STDMETHODIMP CImGuiRenderer::Update(double totalSeconds)
{
    m_now = (float)totalSeconds;
    return S_OK;
}

float m_last = 0;
STDMETHODIMP CImGuiRenderer::Render(ID3D11Device *pDevice)
{
    if (!initialized) {
        CreateDeviceDependentResources(pDevice);
        ImGui_ImplDX11_CreateDeviceObjects();
        initialized = true;
    }

    auto delta = m_now - m_last;
    m_last = m_now;
    Custom_ImGui_ImplDX11_NewFrame(m_width, m_height, delta);

    {
        static float f = 0.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_col);
        if (ImGui::Button("Test Window")) show_test_window ^= 1;
        if (ImGui::Button("Another Window")) show_another_window ^= 1;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello from another window!");
        ImGui::End();
    }

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        ImGui::ShowDemoWindow(&show_test_window);
    }

    ImGui::Render();

    return S_OK;
}

STDMETHODIMP CImGuiRenderer::OnMouseLeftDown()
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[0] = true;
    return S_OK;
}

STDMETHODIMP CImGuiRenderer::OnMouseLeftUp()
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[0] = false;
    return S_OK;
}

STDMETHODIMP CImGuiRenderer::OnMouseMiddleDown()
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[1] = true;
    return S_OK;
}

STDMETHODIMP CImGuiRenderer::OnMouseMiddleUp()
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[1] = false;
    return S_OK;
}

STDMETHODIMP CImGuiRenderer::OnMouseRightDown()
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[2] = true;
    return S_OK;
}

STDMETHODIMP CImGuiRenderer::OnMouseRightUp()
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[2] = false;
    return S_OK;
}

STDMETHODIMP CImGuiRenderer::OnMouseMove(int width, int height)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos.x = (float)width;
    io.MousePos.y = (float)height;
    return S_OK;
}

STDMETHODIMP CImGuiRenderer::OnMouseWheel(int delta)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += (delta > 0 ? +1.0f : -1.0f);
    return S_OK;
}

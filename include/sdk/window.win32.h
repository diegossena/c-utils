#include <sdk/types.h>

#ifdef PLATFORM_WINDOWS

#define COBJMACROS 1
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <initguid.h>
#include <wincodec.h>
#undef COBJMACROS

// window
extern HWND global_window;
extern IWICImagingFactory* global_wic_factory;
// dx11
extern ID3D11Device* global_d3d_device;
extern IDXGISwapChain* global_d3d_swapchain;
extern ID3D11DeviceContext* global_d3d_device_context;

extern ID3D11RasterizerState* global_d3d_rasterizer;
extern ID3D11RenderTargetView* global_d3d_render_target_view;

extern ID3D11VertexShader* global_vertex_shader;
extern ID3D11PixelShader* global_pixel_shader;
extern ID3D11InputLayout* global_input_layout;
extern ID3D11SamplerState* global_sampler_state;

#endif
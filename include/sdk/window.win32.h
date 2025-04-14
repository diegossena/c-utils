#include <sdk/types.h>
#ifndef WINDOW_WIN32_H
#define WINDOW_WIN32_H

#ifdef PLATFORM_WINDOWS

#define COBJMACROS 1
#include <windows.h>
#include <d3d11.h>
#undef COBJMACROS

// window
extern HWND window_id;
// dx11
extern ID3D11Device* d3d_device;
extern IDXGISwapChain* d3d_swapchain;
extern ID3D11DeviceContext* d3d_device_context;

extern ID3D11RasterizerState* d3d_rasterizer;
extern ID3D11RenderTargetView* d3d_render_target_view;

extern ID3D11VertexShader* d3d_vertex_shader;
extern ID3D11PixelShader* d3d_pixel_shader;
extern ID3D11InputLayout* d3d_input_layout;
extern ID3D11SamplerState* d3d_sampler_state;
extern ID3D11BlendState* d3d_blend_state;

extern ID3D11ShaderResourceView* d3d_shader_resource;

extern ID3D11Buffer* vertices_buffer;
extern ID3D11Buffer* indexes_buffer;

#endif

#endif
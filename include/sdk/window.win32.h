#include <sdk/types.h>
#ifndef WINDOW_WIN32_H
#define WINDOW_WIN32_H

#ifdef PLATFORM_WINDOWS

#define COBJMACROS 1
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#undef COBJMACROS

// window
extern HWND window_id;
// dx11
extern ID3D11Device* _d3d_device;
extern IDXGISwapChain* _d3d_swapchain;
extern ID3D11DeviceContext* _d3d_device_context;

extern ID3D11RasterizerState* _d3d_rasterizer;
extern ID3D11RenderTargetView* _d3d_render_target_view;

extern ID3D11VertexShader* _d3d_vertex_shader;
extern ID3D11PixelShader* _d3d_pixel_shader;
extern ID3D11InputLayout* _d3d_input_layout;
extern ID3D11SamplerState* _d3d_sampler_state;
extern ID3D11BlendState* _d3d_blend_state;

extern ID3D11ShaderResourceView* _d3d_shader_resource;

extern ID3D11Buffer* _d3d_vertices_buffer;
extern ID3D11Buffer* _d3d_indexes_buffer;

#endif

#endif
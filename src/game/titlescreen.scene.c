#include <game/titlescreen.scene.h>
#include <game/tilemap.scene.h>
#include <game/hero_home_2f.map.h>

#include <sdk/window.win32.h>

titlescreen_t titlescreen;

export void titlescreen_load() {
  // titlescreen.white = gfx_color_new(1, 1, 1, 1);
  // // title
  // titlescreen.title_style = gfx_textstyle_new(font_family_megaman, 32, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  // // press_space
  // titlescreen.press_space_style = gfx_textstyle_new(font_family_megaman, 26, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  // loaded
  titlescreen.loaded = true;
  global_window_repaint = true;
}
export void titlescreen_unload() {
  titlescreen.loaded = false;
}
export void titlescreen_onkeydown(key_code_t key) {
  switch (key) {
    case KEY_SPACE:
      // transition_load(titlescreen_unload, hero_home_2f_load);
      break;
    default:
      return;
  }
}
export void titlescreen_render() {
  LRESULT result;
  ID3D11Buffer* vertex_buffer;
  // CreateVertexBuffer
  f32 vertices [][4] = {
    {
      0.0f, 0.5f,
      0.f, 1.f
    },
    {
      0.45f, -0.5,
      .5f, .0f
    },
    {
      -0.45f, -0.5f,
      1.f, 1.f
    }
  };
  const u32 vertex_stride = sizeof(vertices[0]);
  const u64 vertex_count = sizeof(vertices) / vertex_stride;
  const u32 vertex_offset = 0;
  D3D11_BUFFER_DESC vb_desc = {
    .ByteWidth = sizeof(vertices),
    .Usage = D3D11_USAGE_DEFAULT,
    .BindFlags = D3D11_BIND_VERTEX_BUFFER,
  };
  D3D11_SUBRESOURCE_DATA vbData = { vertices };
  result = global_d3d_device->lpVtbl->CreateBuffer(
    global_d3d_device, &vb_desc, &vbData, &vertex_buffer
  );
  if (FAILED(result)) {
    error("VertexShader_CreateBuffer", result);
  }
  global_d3d_device_context->lpVtbl->IASetVertexBuffers(global_d3d_device_context, 0, 1, &vertex_buffer, &vertex_stride, &vertex_offset);
  // render
  global_d3d_device_context->lpVtbl->Draw(global_d3d_device_context, vertex_count, 0);
  global_d3d_swapchain->lpVtbl->Present(global_d3d_swapchain, 0, 0);
  // cleanup
  vertex_buffer->lpVtbl->Release(vertex_buffer);
}
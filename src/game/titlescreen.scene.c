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
  buffer_t* vertices_header = buffer_header(global_vertices);
  vertices_header->length = 0;
  if (vertices_header->size >= 4) {
    vertex_t vertices [] = {
      {
        -.5f, -.5f,
        0.f, 1.f
      },
      {
        -.5f, .5,
        .0f, .0f
      },
      {
        .5f, .5f,
        1.f, 0.f
      },
      {
        .5f, -.5f,
        1.f, 1.f
      }
    };
    const u64 vertices_length = sizeof(vertices) / sizeof(vertex_t);
    for (u64 i = 0; i < vertices_length; i++) {
      global_vertices[vertices_header->length++] = vertices[i];
    }
  }
  D3D11_MAPPED_SUBRESOURCE ms;
  global_d3d_device_context->lpVtbl->Map(
    global_d3d_device_context, (ID3D11Resource*)global_vertex_buffer, 0,
    D3D11_MAP_WRITE_DISCARD, 0, &ms
  );
  memory_copy(ms.pData, global_vertices, sizeof(vertex_t) * vertices_header->length);
  global_d3d_device_context->lpVtbl->Unmap(
    global_d3d_device_context, (ID3D11Resource*)global_vertex_buffer, 0
  );
  // CreateIndexBuffer
  u32 indices [] = {
    0, 1, 2,
    0, 2, 3
  };
  const u64 index_count = sizeof(indices) / sizeof(u32);
  D3D11_BUFFER_DESC index_desc = {
    .ByteWidth = sizeof(indices),
    .Usage = D3D11_USAGE_DEFAULT,
    .BindFlags = D3D11_BIND_INDEX_BUFFER,
    .CPUAccessFlags = 0,
    .MiscFlags = 0
  };
  D3D11_SUBRESOURCE_DATA index_data = { indices };
  ID3D11Buffer* index_buffer;
  result = global_d3d_device->lpVtbl->CreateBuffer(
    global_d3d_device, &index_desc, &index_data, &index_buffer
  );
  if (FAILED(result)) {
    error("IndexShader_CreateBuffer", result);
  }
  global_d3d_device_context->lpVtbl->IASetIndexBuffer(global_d3d_device_context, index_buffer, DXGI_FORMAT_R32_UINT, 0);
  // render
  global_d3d_device_context->lpVtbl->DrawIndexed(global_d3d_device_context, index_count, 0, 0);
  global_d3d_swapchain->lpVtbl->Present(global_d3d_swapchain, 0, 0);
  // cleanup
  index_buffer->lpVtbl->Release(index_buffer);
}
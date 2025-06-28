struct vs_out {
  float4 position : SV_POSITION;
  float4 uv_color : TEXCOORD;
};

Texture2D texture : register(t0);
SamplerState sample_state : register(s0);

vs_out vs_main(float2 position : POSITION, float4 uv_color : TEXCOORD) {
  vs_out output;
  output.position = float4(position, 1.f, 1.f);
  output.uv_color = uv_color;
  return output;
}
float4 ps_main(vs_out input) : SV_TARGET {
  return input.uv_color.a > 0
    ? input.uv_color
    : texture.Sample(sample_state, input.uv_color.xy);
}

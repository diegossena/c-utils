struct vs_out {
  float4 position : SV_POSITION;
  float2 uv : TEXCOORD;
};

Texture2D texture_2d : register(t0);
SamplerState sample_state : register(s0);

vs_out vs_main(float2 position : POSITION, float2 uv : TEXCOORD) {
  vs_out output;
  output.position = float4(position, 1.f, 1.f);
  output.uv = uv;
  return output;
}
float4 ps_main(vs_out input) : SV_TARGET {
  float4 pixel_color = texture_2d.Sample(sample_state, input.uv);
  return pixel_color;
}

struct VS_INPUT {
  float2 pos : POSITION;
  float2 uv : TEXCOORD0;
};
struct PS_INPUT {
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD0;
};

Texture2D tex : register(t0);
SamplerState sample_linear : register(s0);

PS_INPUT vs_main(VS_INPUT input) {
  PS_INPUT output;
  output.pos = float4(input.pos, 1.0, 1.0);
  output.uv = input.uv;
  return output;
}
float4 ps_main(VS_INPUT input) : SV_TARGET {
  return tex.Sample(sample_linear, input.uv);
}

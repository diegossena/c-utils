cbuffer cBuffer : register(b0) {
  float4x4 mat;
}

struct VS_Input {
    float3 pos : Position;
    float4 color : Color;
};

struct VS_Output {
    float4 position : SV_POSITION;
    float4 color : Position;
};

VS_Output vs_main(VS_Input input) {
  VS_Output output;
  output.position = mul(mat, float4(input.pos, 1.f));
  output.color = input.color;    
  return output;
}

float4 ps_main(VS_Output input) : SV_TARGET {
  return input.color;   
}
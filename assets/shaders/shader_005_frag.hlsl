cbuffer ConstantBuffer0 : register(b0)
{
    float4 u_Time;
}

cbuffer ConstantBuffer1 : register(b1)
{
    float4 u_Color;
}

Texture2D Texture : register(t0);
Texture2D Texture2 : register(t1);
SamplerState ss;

// Pixel Shader
float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float2 displacement = Texture2.Sample(ss, texcoord / 6.0).xy;
    float t = texcoord.y + displacement.y * 0.1 - 0.15 + (sin(texcoord.x * 60.0 + u_Time.x) * 0.005);
    return u_Color * Texture.Sample(ss, float2(texcoord.x, t));
}

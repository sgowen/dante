cbuffer ConstantBuffer0 : register(b0)
{
    float4 u_Color;
}

// Pixel Shader
float4 main(float4 position : SV_POSITION) : SV_TARGET
{
	return u_Color;
}

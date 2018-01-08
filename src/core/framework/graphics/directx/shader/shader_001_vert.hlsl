cbuffer ConstantBuffer
{
	float4x4 matFinal;
}

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

// Vertex Shader
VOut main(float2 position : a_Position, float4 color : a_Color)
{
    VOut output;

	output.position = mul(matFinal, float4(position, 0, 1));
	output.color = color;

	return output;
}

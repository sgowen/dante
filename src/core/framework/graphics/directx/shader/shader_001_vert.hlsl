cbuffer ConstantBuffer
{
	float4x4 u_Matrix;
}

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VOut main(float2 position : a_Position, float4 color : a_Color)
{
    VOut output;

	output.position = mul(u_Matrix, float4(position, 0, 1));
	output.color = color;

	return output;
}

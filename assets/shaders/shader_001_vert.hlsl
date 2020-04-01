cbuffer ConstantBuffer0 : register(b0)
{
	float4x4 u_Matrix;
}

struct VOut
{
	float4 position : SV_POSITION;
};

VOut main(float2 position : a_Position)
{
    VOut output;
	output.position = mul(u_Matrix, float4(position, 0, 1));

	return output;
}

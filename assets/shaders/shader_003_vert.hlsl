cbuffer ConstantBuffer0 : register(b0)
{
	float4x4 u_Matrix;
}

struct VOut
{
	float4 position : SV_POSITION;
	float2 texcoord: TEXCOORD;
};

VOut main(float2 position : a_Position, float2 texcoord : a_TexCoord)
{
    VOut output;
    output.position = mul(u_Matrix, float4(position, 0, 1)); // transform the vertex from 3D to 2D
	output.texcoord = texcoord; // set the texture coordinates, unmodified

    return output;
}

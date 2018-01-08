cbuffer ConstantBuffer : register(b0)
{
	float4x4 matFinal;
}

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord: TEXCOORD;        // texture coordinates
};

// Vertex Shader
VOut main(float2 position : a_Position, float4 color : a_Color, float2 texcoord : a_TexCoord)
{
    VOut output;
    output.position = mul(matFinal, float4(position, 0, 1)); // transform the vertex from 3D to 2D
	output.color = color;
	output.texcoord = texcoord; // set the texture coordinates, unmodified

    return output;
}

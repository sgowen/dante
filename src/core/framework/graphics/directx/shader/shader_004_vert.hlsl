cbuffer ConstantBuffer : register(b0)
{
	float4x4 u_Matrix;
}

cbuffer ConstantBuffer : register(b1)
{
    float4x4 u_Lights; //light positions, normalized
}

struct VOut
{
	float4 position : SV_POSITION;
	float2 texcoord: TEXCOORD;
    float4x4 lights: LIGHTS;
};

VOut main(float2 position : a_Position, float4 color : a_Color, float2 texcoord : a_TexCoord)
{
    VOut output;
    output.position = mul(u_Matrix, float4(position, 0, 1)); // transform the vertex from 3D to 2D
	output.color = color;
	output.texcoord = texcoord; // set the texture coordinates, unmodified

    output.lights = u_Lights;
    for (int i = 0; i < 4; i++)
    {
        float4 lightPosIn = u_Lights[i];
        if (lightPosIn.w > 0.0)
        {
            float4 light = float4(lightPosIn.x, lightPosIn.y, 0, 1);
            float4 lightNorm = mul(u_Matrix * light);

            float2 c = float2(lightNorm.x, lightNorm.y);
            float2 lightPos = (c + 1.0) / 2.0;
            output.lights[i] = float4(lightPos.x, lightPos.y, lightPosIn.z, 1);
        }
    }

    return output;
}

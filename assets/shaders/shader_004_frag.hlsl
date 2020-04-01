cbuffer ConstantBuffer0 : register(b0)
{
    float4x4 u_Matrix;
}

cbuffer ConstantBuffer1 : register(b1)
{
    float4 u_LightPositions[12]; //light positions
}

cbuffer ConstantBuffer2 : register(b2)
{
    float4 u_LightColors[12];
}

cbuffer ConstantBuffer3 : register(b3)
{
    float4 u_AmbientColor;
}

cbuffer ConstantBuffer4 : register(b4)
{
    float4 u_Falloff;
}

cbuffer ConstantBuffer5 : register(b5)
{
    int4 u_NumLights;
}

Texture2D Texture : register(t0); //diffuse map
Texture2D TextureN : register(t1); //normal map
SamplerState ss;

// Pixel Shader
float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    //RGBA of our diffuse color
    float4 DiffuseColor = Texture.Sample(ss, texcoord);

    //RGB of our normal map
    float3 NormalMap = TextureN.Sample(ss, texcoord).rgb;
	NormalMap.g = 1.0 - NormalMap.g;

    //pre-multiply ambient color with intensity
    float3 Ambient = u_AmbientColor.rgb * u_AmbientColor.a;

    float3 Sum = float3(0.0, 0.0, 0.0);
    for (int i = 0; i < u_NumLights.x; ++i)
    {
        //The delta position of light
        float4 lightPosIn = u_LightPositions[i];
        float4 light = float4(lightPosIn.x, lightPosIn.y, 0, 1);
        float4 lightNorm = mul(u_Matrix, light);

        float2 c = float2(lightNorm.x, lightNorm.y);
        float2 lightPos = (c + 1.0) / 2.0;
        lightPos.y = 1 - lightPos.y;

        float3 LightDir = float3(lightPos.xy - texcoord, lightPosIn.z);

        //Determine distance (used for attenuation) BEFORE we normalize our LightDir
        float D = length(LightDir);

        //normalize our vectors
        float3 N = normalize(NormalMap * 2.0 - 1.0);
        float3 L = normalize(LightDir);

        //Pre-multiply light color with intensity
        //Then perform "N dot L" to determine our diffuse term
        float3 Diffuse = (u_LightColors[i].rgb * u_LightColors[i].a) * max(dot(N, L), 0.0);

        //calculate attenuation
        float Attenuation = 1.0 / (u_Falloff.x + (u_Falloff.y*D) + (u_Falloff.z*D*D));

        //the calculation which brings it all together
        float3 Intensity = Diffuse * Attenuation;

        Sum += Intensity;
    }

    Sum += Ambient;
    return float4(Sum * DiffuseColor.rgb, DiffuseColor.a);
}

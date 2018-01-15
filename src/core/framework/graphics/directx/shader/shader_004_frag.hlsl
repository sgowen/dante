cbuffer ConstantBuffer : register(b2)
{
    float4 u_LightColor;
}

cbuffer ConstantBuffer : register(b3)
{
    float4 u_AmbientColor;
}

cbuffer ConstantBuffer : register(b4)
{
    float4 u_Falloff;
}

cbuffer ConstantBuffer : register(b5)
{
    float2 u_Resolution;
}

Texture2D u_TextureUnit; //diffuse map
Texture2D u_NormalMapUnit; //normal map
SamplerState ss;

// Pixel Shader
float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD, float4x4 lights : LIGHTS) : SV_TARGET
{
    //RGBA of our diffuse color
    float4 DiffuseColor = u_TextureUnit.Sample(ss, texcoord);

    //RGB of our normal map
    float3 NormalMap = u_NormalMapUnit.Sample(ss, texcoord).rgb;

    //pre-multiply ambient color with intensity
    float3 Ambient = u_AmbientColor.rgb * u_AmbientColor.a;

    float3 Sum = float3(0.0);
    for (int i = 0; i < 4; i++)
    {
        //The delta position of light
        float4 lightPos = lights[i];
        if (lightPos.w > 0.0)
        {
            float3 LightDir = float3(lightPos.xy - (position.xy / u_Resolution.xy), lightPos.z);

            //Correct for aspect ratio
            LightDir.x *= u_Resolution.x / u_Resolution.y;

            //Determine distance (used for attenuation) BEFORE we normalize our LightDir
            float D = length(LightDir);

            //normalize our vectors
            float3 N = normalize(NormalMap * 2.0 - 1.0);
            float3 L = normalize(LightDir);

            //Pre-multiply light color with intensity
            //Then perform "N dot L" to determine our diffuse term
            float3 Diffuse = (u_LightColor.rgb * u_LightColor.a) * max(dot(N, L), 0.0);

            //calculate attenuation
            float Attenuation = 1.0 / (u_Falloff.x + (u_Falloff.y*D) + (u_Falloff.z*D*D));

            //the calculation which brings it all together
            float3 Intensity = Diffuse * Attenuation;

            Sum += Intensity;
        }
    }

    Sum += Ambient;
    return float4(Sum * DiffuseColor.rgb, DiffuseColor.a);
}

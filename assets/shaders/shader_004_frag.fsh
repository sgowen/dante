#ifdef GL_ES
precision mediump float;
#endif

//attributes from vertex shader
varying vec2 v_TexCoord;

//values used for shading algorithm...
uniform mat4 u_Matrix;
uniform vec4 u_LightPositions[12]; //light positions
uniform vec4 u_LightColors[12]; //light RGBA -- alpha is intensity
uniform vec4 u_AmbientColor;    //ambient RGBA -- alpha is intensity
uniform vec4 u_Falloff;         //attenuation coefficients
uniform ivec4 u_NumLights;

//our texture samplers
uniform sampler2D u_Texture;   //diffuse map
uniform sampler2D u_NormalMap;   //normal map

void main()
{
    //RGBA of our diffuse color
    vec4 DiffuseColor = texture2D(u_Texture, v_TexCoord);
    
    //RGB of our normal map
    vec3 NormalMap = texture2D(u_NormalMap, v_TexCoord).rgb;
    
    //pre-multiply ambient color with intensity
    vec3 Ambient = u_AmbientColor.rgb * u_AmbientColor.a;
    
    vec3 Sum = vec3(0.0);
    for (int i = 0; i < u_NumLights.x; ++i)
    {
        //The delta position of light
        vec4 lightPosIn = u_LightPositions[i];
        vec4 light = vec4(lightPosIn.x, lightPosIn.y, 0, 1);
        vec4 lightNorm = u_Matrix * light;
        
        vec2 c = vec2(lightNorm.x, lightNorm.y);
        vec2 lightPos = (c + 1.0) / 2.0;
        
        vec3 LightDir = vec3(lightPos.xy - v_TexCoord, lightPosIn.z);
        
        //Determine distance (used for attenuation) BEFORE we normalize our LightDir
        float D = length(LightDir);
        
        //normalize our vectors
        vec3 N = normalize(NormalMap * 2.0 - 1.0);
        vec3 L = normalize(LightDir);
        
        //Pre-multiply light color with intensity
        //Then perform "N dot L" to determine our diffuse term
        vec3 Diffuse = (u_LightColors[i].rgb * u_LightColors[i].a) * max(dot(N, L), 0.0);
        
        //calculate attenuation
        float Attenuation = 1.0 / (u_Falloff.x + (u_Falloff.y*D) + (u_Falloff.z*D*D));
        
        //the calculation which brings it all together
        vec3 Intensity = Diffuse * Attenuation;
        
        Sum += Intensity;
    }
    
    Sum += Ambient;
    gl_FragColor = vec4(Sum * DiffuseColor.rgb, DiffuseColor.a);
}

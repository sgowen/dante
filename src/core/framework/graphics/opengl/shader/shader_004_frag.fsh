#ifdef GL_ES
precision mediump float;
#endif

//attributes from vertex shader
varying vec2 v_TexCoord;
varying mat4 v_Lights; //light positions, normalized

//values used for shading algorithm...
uniform vec4 u_LightColor;      //light RGBA -- alpha is intensity
uniform vec4 u_AmbientColor;    //ambient RGBA -- alpha is intensity
uniform vec4 u_Falloff;         //attenuation coefficients
uniform vec2 u_Resolution;

//our texture samplers
uniform sampler2D u_TextureUnit;   //diffuse map
uniform sampler2D u_NormalMapUnit;   //normal map

void main()
{
    //RGBA of our diffuse color
    vec4 DiffuseColor = texture2D(u_TextureUnit, v_TexCoord);
    
    //RGB of our normal map
    vec3 NormalMap = texture2D(u_NormalMapUnit, v_TexCoord).rgb;
    
    //pre-multiply ambient color with intensity
    vec3 Ambient = u_AmbientColor.rgb * u_AmbientColor.a;
    
    vec3 Sum = vec3(0.0);
    for (int i = 0; i < 4; i++)
    {
        //The delta position of light
        vec4 lightPos = v_Lights[i];
        if (lightPos.w > 0.0)
        {
            vec3 LightDir = vec3(lightPos.xy - (gl_FragCoord.xy / u_Resolution.xy), lightPos.z);
            
            //Correct for aspect ratio
            LightDir.x *= u_Resolution.x / u_Resolution.y;
            
            //Determine distance (used for attenuation) BEFORE we normalize our LightDir
            float D = length(LightDir);
            
            //normalize our vectors
            vec3 N = normalize(NormalMap * 2.0 - 1.0);
            vec3 L = normalize(LightDir);
            
            //Pre-multiply light color with intensity
            //Then perform "N dot L" to determine our diffuse term
            vec3 Diffuse = (u_LightColor.rgb * u_LightColor.a) * max(dot(N, L), 0.0);
            
            //calculate attenuation
            float Attenuation = 1.0 / (u_Falloff.x + (u_Falloff.y*D) + (u_Falloff.z*D*D));
            
            //the calculation which brings it all together
            vec3 Intensity = Diffuse * Attenuation;
            
            Sum += Intensity;
        }
    }
    
    Sum += Ambient;
    gl_FragColor = vec4(Sum * DiffuseColor.rgb, DiffuseColor.a);
}

#ifdef GL_ES
precision mediump float;
#endif

//our texture samplers
uniform sampler2D u_TextureUnit;   //diffuse map
uniform sampler2D u_NormalMapUnit;   //normal map

//attributes from vertex shader
varying vec4 v_Color;
varying vec2 v_TexCoord;
varying vec3 v_LightPos;

//values used for shading algorithm...
uniform vec4 u_LightColor;      //light RGBA -- alpha is intensity
uniform vec4 u_AmbientColor;    //ambient RGBA -- alpha is intensity
uniform vec3 u_Falloff;         //attenuation coefficients

void main()
{
    //RGBA of our diffuse color
    vec4 DiffuseColor = texture2D(u_TextureUnit, v_TexCoord);
    
    //RGB of our normal map
    vec3 NormalMap = texture2D(u_NormalMapUnit, v_TexCoord).rgb;
    
    vec2 Resolution = vec2(1440.0, 900.0);
    
    //The delta position of light
    vec3 LightDir = vec3(v_LightPos.xy - (gl_FragCoord.xy / Resolution.xy), v_LightPos.z);
    
    //Correct for aspect ratio
    LightDir.x *= Resolution.x / Resolution.y;
    
    //Determine distance (used for attenuation) BEFORE we normalize our LightDir
    float D = length(LightDir);
    
    //normalize our vectors
    vec3 N = normalize(NormalMap * 2.0 - 1.0);
    vec3 L = normalize(LightDir);
    
    //Pre-multiply light color with intensity
    //Then perform "N dot L" to determine our diffuse term
    vec3 Diffuse = (u_LightColor.rgb * u_LightColor.a) * max(dot(N, L), 0.0);
    
    //pre-multiply ambient color with intensity
    vec3 Ambient = u_AmbientColor.rgb * u_AmbientColor.a;
    
    //calculate attenuation
    float Attenuation = 1.0 / (u_Falloff.x + (u_Falloff.y*D) + (u_Falloff.z*D*D));
    
    //the calculation which brings it all together
    vec3 Intensity = Ambient + Diffuse * Attenuation;
    vec3 FinalColor = DiffuseColor.rgb * Intensity;
    gl_FragColor = v_Color * vec4(FinalColor, DiffuseColor.a);
}

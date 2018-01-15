uniform mat4 u_Matrix;
uniform mat4 u_Lights; //light positions, normalized

attribute vec2 a_Position;
attribute vec4 a_Color;
attribute vec2 a_TexCoord;

varying vec2 v_TexCoord;
varying mat4 v_Lights; //light positions, normalized

void main()
{
    gl_Position = u_Matrix * vec4(a_Position, 0.0, 1.0);
    
    vec2 pos = vec2(gl_Position.x, gl_Position.y);
    v_TexCoord = (pos + 1.0) / 2.0;
    
    v_Lights = u_Lights;
    for (int i = 0; i < 4; i++)
    {
        vec4 lightPosIn = u_Lights[i];
        if (lightPosIn.w > 0.0)
        {
            vec4 light = vec4(lightPosIn.x, lightPosIn.y, 0, 1);
            vec4 lightNorm = u_Matrix * light;
            
            vec2 c = vec2(lightNorm.x, lightNorm.y);
            vec2 lightPos = (c + 1.0) / 2.0;
            v_Lights[i] = vec4(lightPos.x, lightPos.y, lightPosIn.z, 1);
        }
    }
}

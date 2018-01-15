uniform mat4 u_Matrix;
uniform vec3 u_LightPos;        //light position, normalized

attribute vec2 a_Position;
attribute vec4 a_Color;
attribute vec2 a_TexCoord;

varying vec4 v_Color;
varying vec2 v_TexCoord;
varying vec3 v_LightPos;

void main()
{
    v_Color = a_Color;
    
    gl_Position = u_Matrix * vec4(a_Position, 0.0, 1.0);
    
    vec2 pos = vec2(gl_Position.x, gl_Position.y);
    v_TexCoord = (pos + 1.0) / 2.0;
    
    vec4 center = vec4(u_LightPos.x, u_LightPos.y, 0, 1);
    vec4 screenCenter = u_Matrix * center;
    
    vec2 c = vec2(screenCenter.x, screenCenter.y);
    vec2 lightPos = (c + 1.0) / 2.0;
    v_LightPos = vec3(lightPos.x, lightPos.y, u_LightPos.z);
}

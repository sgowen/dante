uniform mat4 u_Matrix;

attribute vec4 a_Position;
attribute vec4 a_Color;
attribute vec2 a_TexCoord;

varying vec4 v_Color;
varying vec2 v_TexCoord;

void main()
{
    gl_Position = u_Matrix * a_Position;
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
}

uniform mat4 u_Matrix;

attribute vec2 a_Position;

void main()
{
    gl_Position = u_Matrix * vec4(a_Position, 0.0, 1.0);
}

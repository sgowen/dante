#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_Texture;
uniform vec4 u_Color;

varying vec2 v_TexCoord;

void main()
{
    gl_FragColor = u_Color * texture2D(u_Texture, v_TexCoord);
}

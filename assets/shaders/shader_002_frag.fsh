#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_Texture;

varying vec2 v_TexCoord;

void main()
{
    gl_FragColor = texture2D(u_Texture, v_TexCoord);
}

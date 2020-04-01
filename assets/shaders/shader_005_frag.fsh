#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_Texture;
uniform sampler2D u_Texture2;
uniform vec4 u_Color;
uniform vec4 u_Time;

varying vec2 v_TexCoord;

void main()
{
    vec2 displacement = texture2D(u_Texture2, v_TexCoord / 6.0).xy;
    float t = v_TexCoord.y + displacement.y * 0.1 - 0.15 + (sin(v_TexCoord.x * 60.0 + u_Time.x) * 0.005);
    gl_FragColor = u_Color * texture2D(u_Texture, vec2(v_TexCoord.x, t));
}

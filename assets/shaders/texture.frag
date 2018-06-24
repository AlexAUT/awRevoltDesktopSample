varying vec2 uv;

uniform sampler2D baseColor_tex;

void main()
{
  gl_FragColor = texture2D(baseColor_tex, uv);
//  gl_FragColor = vec4(uv.x, uv.y, 0.0, 1.0);
}

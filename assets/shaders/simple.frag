varying vec3 color;

void main()
{
  gl_FragColor = vec4(abs(color), 1.0);
}

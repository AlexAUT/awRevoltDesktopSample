attribute vec3 vPosition;

uniform mat4 mvp_matrix;

void main()
{
  gl_Position = mvp_matrix * vec4(vPosition, 1.0);
}

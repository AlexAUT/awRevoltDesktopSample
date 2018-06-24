attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexCoord;
attribute vec4 vBoneIds;
attribute vec4 vBonesWeights;

uniform mat4 mvp_matrix;

varying vec3 color;

void main()
{
  color = vNormal;
  gl_Position = mvp_matrix * vec4(vPosition, 1.0);
}

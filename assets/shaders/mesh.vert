attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexCoord;
attribute vec4 vBoneIds;
attribute vec4 vBoneWeights;

uniform mat4 mvp_matrix;
uniform mat4 vp_matrix;
uniform mat4 vm_matrix;
uniform mat4 m_matrix;
uniform mat4 shadow_mvp_biased;
uniform bool has_bones;

const int MAX_BONES = 58;
uniform mat4 bones[MAX_BONES];

varying vec4 pos_shadowmap;
varying vec2 uv;
varying vec3 normal;

mat3 cinverse(mat3 m) {
  float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
  float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
  float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

  float b01 = a22 * a11 - a12 * a21;
  float b11 = -a22 * a10 + a12 * a20;
  float b21 = a21 * a10 - a11 * a20;

  float det = a00 * b01 + a01 * b11 + a02 * b21;

  return mat3(b01, (-a22 * a01 + a02 * a21), (a12 * a01 - a02 * a11),
              b11, (a22 * a00 - a02 * a20), (-a12 * a00 + a02 * a10),
              b21, (-a21 * a00 + a01 * a20), (a11 * a00 - a01 * a10)) / det;
}
mat3 ctranspose(mat3 m) {
  return mat3(m[0][0], m[1][0], m[2][0],
              m[0][1], m[1][1], m[2][1],
              m[0][2], m[1][2], m[2][2]);
}

void main()
{
  mat4 modelMatrix;
  if(has_bones)
  {
    mat4 boneMatrix = bones[int(vBoneIds[0])] * vBoneWeights[0];
    boneMatrix += bones[int(vBoneIds[1])] * vBoneWeights[1];
    boneMatrix += bones[int(vBoneIds[2])] * vBoneWeights[2];
    boneMatrix += bones[int(vBoneIds[3])] * vBoneWeights[3];
    modelMatrix = m_matrix * boneMatrix;
  }
  else
    modelMatrix = m_matrix;

  uv = vTexCoord;
  gl_Position = vp_matrix * modelMatrix * vec4(vPosition, 1.0);
  
  pos_shadowmap = shadow_mvp_biased * modelMatrix * vec4(vPosition, 1.0);
  mat3 normalTransform = cinverse(ctranspose(mat3(modelMatrix)));
  normal = normalTransform * vNormal;
}

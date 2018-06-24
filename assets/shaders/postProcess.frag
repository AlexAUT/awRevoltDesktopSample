varying vec2 uv;

uniform sampler2D postProcess_tex;

float linearize_depth(float original_depth) {
    float near = 0.1;
    float far = 1000.0;
    return (2.0 * near) / (far + near - original_depth * (far - near));
}

void main()
{
  gl_FragColor = texture2D(postProcess_tex, uv); 

}

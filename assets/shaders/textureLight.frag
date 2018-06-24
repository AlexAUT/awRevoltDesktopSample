struct DirectionalLight
{
  vec3 dir;
  vec4 color;
  float energy;
};

varying vec4 pos_shadowmap;
varying vec2 uv;
varying vec3 normal;

uniform vec3 camera_position;
uniform vec3 camera_viewDir;

const int MAX_NUM_DIR_LIGHTS = 1;
uniform DirectionalLight dirLights[MAX_NUM_DIR_LIGHTS];

uniform sampler2D baseColor_tex;
uniform sampler2D shadow_tex;

void main()
{
  vec4 color;
  vec4 diffuseC = texture2D(baseColor_tex, uv);
  for(int i = 0; i < MAX_NUM_DIR_LIGHTS; i++)
  {
    DirectionalLight l = dirLights[i];
    float cosTheta = max(0.0, dot(-l.dir, normal));
    float visibility = 1.f;
    
    float bias = 0.005 * tan(acos(cosTheta));
    bias = clamp(bias, 0.0, 0.01);
    vec2 offsetPerPixel = vec2(1.0 / 4048.0, 1.0 / 4048.0);
    for(int x=-1; x<=1; x++){
        for(int y=-1; y<=1; y++){
          if(texture2D(shadow_tex, pos_shadowmap.xy + offsetPerPixel * vec2(x, y)).r < pos_shadowmap.z - bias)
            visibility -= 1.0 / 9.0;
        }
    }
    //visibility = 1.0;
    float diffuse = max(visibility * cosTheta, 0.15);  
    color = diffuseC * diffuse * dirLights[i].energy;
  }
  color.a = 1.0;
  gl_FragColor = texture2D(baseColor_tex, uv);
  //gl_FragColor = vec4(diffuse, diffuse, diffuse, 1.0);
  gl_FragColor = color;
  //gl_FragColor = vec4(normal, 1.0);
}

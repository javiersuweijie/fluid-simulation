varying vec2 UV;
uniform sampler2D Texture0; //density field
uniform sampler2D Texture1; //velocity field
uniform float pixelSize;
float timestep = 0.01;

vec4 bilinear_mix(sampler2D texture, vec2 st, float gridSize)
{
  vec2 st2 = floor(st / gridSize) * gridSize;
  vec2 t = st - st2; 
 
  vec4 tex00 = texture2D(texture, st2 - 1.0); 
  vec4 tex10 = texture2D(texture, vec2(st2.x + 1.0, st2.y - 1.0));
  vec4 tex01 = texture2D(texture, vec2(st2.x - 1.0, st2.y + 1.0));
  vec4 tex11 = texture2D(texture, st2 + 1.0);

  // bilinear interpolation
  return mix(mix(tex00, tex10, t.x), mix(tex01, tex11, t.x), t.y);
}

#define texUV(s,t) vec2(UV.x+(s),UV.y+(t))

void main() {
  vec2 pos = UV - timestep * (texture2D(Texture1, UV).xy);
  gl_FragColor = bilinear_mix(Texture0, pos, pixelSize);

}

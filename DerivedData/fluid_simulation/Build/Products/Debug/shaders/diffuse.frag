varying vec2 UV;
uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform float pixelSize;

#define texUV(s,t) vec2(UV.x+(s),UV.y+(t))

void main(){
  vec4 d0 = texture2D(Texture0, UV);
  vec4 d1 = texture2D(Texture1, vec2(UV.x+pixelSize, UV.y));
  vec4 d2 = texture2D(Texture1, vec2(UV.x-pixelSize, UV.y));
  vec4 d3 = texture2D(Texture1, vec2(UV.x, UV.y+pixelSize));
  vec4 d4 = texture2D(Texture1 ,vec2(UV.x, UV.y-pixelSize));
  float a = 0.163840;
  vec4 d = (d0 + a*(d1+d2+d3+d4))/(1.0+(4.0*a));
  /*vec4 d = d0+a*(d1+d2+d3+d4-4.0*d0);*/
  gl_FragColor = vec4(d);
}

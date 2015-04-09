varying vec2 UV;
uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform float pixelSize;

#define texUV(s,t) vec2(UV.x+(s),UV.y+(t))

void main(){
  float d0 = texture2D(Texture0, UV).x;
  float d1 = texture2D(Texture1, vec2(UV.x+pixelSize, UV.y)).x;
  float d2 = texture2D(Texture1, vec2(UV.x-pixelSize, UV.y)).x;
  float d3 = texture2D(Texture1, vec2(UV.x, UV.y+pixelSize)).x;
  float d4 = texture2D(Texture1 ,vec2(UV.x, UV.y-pixelSize)).x;
  float a = 0.163840;
  float d = (d0 + a*(d1+d2+d3+d4))/(1.0+(4.0*a));
  gl_FragData[0] = vec4(d,0.0,0.0,1.0);
}

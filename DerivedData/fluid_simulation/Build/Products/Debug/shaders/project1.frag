varying vec2 UV;
uniform sampler2D Texture0;
uniform float pixelSize;
#define texUV(s,t) texture2D(Texture0,vec2(UV.x+(s),UV.y+(t)))

void main() {
  gl_FragColor = vec4(-0.5 * (texUV(pixelSize,0.0).x - texUV(-pixelSize,0.0).x + texUV(0.0,pixelSize).y - texUV(0.0, -pixelSize).y) * pixelSize, 1.0,1.0,1.0);
}

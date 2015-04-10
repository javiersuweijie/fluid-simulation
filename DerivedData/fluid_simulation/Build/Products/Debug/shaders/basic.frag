varying vec2 UV;
uniform sampler2D Texture0;
uniform float pixelSize;

void main() {
  vec4 d0 = texture2D(Texture0, UV);
  vec4 d1 = texture2D(Texture0, vec2(UV.x+pixelSize, UV.y));
  vec4 d2 = texture2D(Texture0, vec2(UV.x-pixelSize, UV.y));
  vec4 d3 = texture2D(Texture0, vec2(UV.x, UV.y+pixelSize));
  vec4 d4 = texture2D(Texture0, vec2(UV.x, UV.y-pixelSize));

  /*gl_FragColor = (d0+d1+d2+d3+d4)/5.0;*/
  gl_FragColor = d0;

}

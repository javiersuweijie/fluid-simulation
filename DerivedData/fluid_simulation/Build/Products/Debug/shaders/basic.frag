varying vec2 UV;
uniform sampler2D Texture0;

void main() {
  gl_FragColor = texture2D(Texture0, UV);
}

attribute vec2 tex;
attribute vec3 position;

varying vec2 UV;

void main() {
  UV = tex;
  gl_Position = vec4(position, 1.0);
}

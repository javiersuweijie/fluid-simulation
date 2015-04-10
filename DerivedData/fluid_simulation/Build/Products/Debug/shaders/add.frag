varying vec2 UV;
uniform sampler2D Texture0;
uniform sampler2D Texture1;

void main(){
  vec4 d0 = texture2D(Texture0, UV);
  vec4 d1 = texture2D(Texture1, UV);
  gl_FragColor = vec4((d0+d1).xyz,1.0);
}

varying vec2 UV;
uniform sampler2D Texture;

void main(){
  float texRed = texture2D(Texture, UV).x;
  gl_FragColor = vec4(0.0,texRed,0.0,1.0);
}

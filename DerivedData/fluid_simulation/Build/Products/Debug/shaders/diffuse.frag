varying vec2 UV;
uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform float pixelSize;
uniform float textureSize;
uniform float a;
uniform float c;

#define texUV(s,t) vec2(UV.x+(s),UV.y+(t))

void main(){
  vec4 d0 = texture2D(Texture0, UV);
  vec4 d1 = texture2D(Texture1, vec2(UV.x+pixelSize, UV.y));
  vec4 d2 = texture2D(Texture1, vec2(UV.x-pixelSize, UV.y));
  vec4 d3 = texture2D(Texture1, vec2(UV.x, UV.y+pixelSize));
  vec4 d4 = texture2D(Texture1, vec2(UV.x, UV.y-pixelSize));
  vec4 d = (d0 + a*(d1+d2+d3+d4))/c;
  gl_FragColor = vec4(d.xyz,1.0);

  /*vec4 d0 = texture2D(Texture0, UV);*/
  /*vec4 d1 = texture2D(Texture0, vec2(UV.x+pixelSize, UV.y));*/
  /*vec4 d2 = texture2D(Texture0, vec2(UV.x-pixelSize, UV.y));*/
  /*vec4 d3 = texture2D(Texture0, vec2(UV.x, UV.y+pixelSize));*/
  /*vec4 d4 = texture2D(Texture0, vec2(UV.x, UV.y-pixelSize));*/
  /*float a = pixelSize*pixelSize/0.00001;*/
  /*vec4 d = (d1+d2+d3+d4+a*d0)/(4.0+a);*/
  /*vec4 d = (d1+d2+d3+d4)/4.0 - d0/4.0;*/
  /*gl_FragColor = vec4(d);*/

  /*int count = 0;*/

  /*vec4 C = texture2D( Texture0, UV );*/
  /*vec4 E = texture2D( Texture0, vec2(UV.x + pixelSize, UV.y) );*/
  /*vec4 N = texture2D( Texture0, vec2(UV.x, UV.y + pixelSize) );*/
  /*vec4 W = texture2D( Texture0, vec2(UV.x - pixelSize, UV.y) );*/
  /*vec4 S = texture2D( Texture0, vec2(UV.x, UV.y - pixelSize) );*/
  /*vec4 NE = texture2D( Texture0, vec2(UV.x + pixelSize, UV.y + pixelSize) );*/
  /*vec4 NW = texture2D( Texture0, vec2(UV.x - pixelSize, UV.y + pixelSize) );*/
  /*vec4 SE = texture2D( Texture0, vec2(UV.x + pixelSize, UV.y - pixelSize) );*/
  /*vec4 SW = texture2D( Texture0, vec2(UV.x - pixelSize, UV.y - pixelSize) );*/
 
  /*if (E.r == 1.0) { count++; }*/
  /*if (N.r == 1.0) { count++; }*/
  /*if (W.r == 1.0) { count++; }*/
  /*if (S.r == 1.0) { count++; }*/
  /*if (NE.r == 1.0) { count++; }*/
  /*if (NW.r == 1.0) { count++; }*/
  /*if (SE.r == 1.0) { count++; }*/
  /*if (SW.r == 1.0) { count++; }*/
 
  /*if ( (C.r == 0.0 && count == 3) ||*/
       /*(C.r == 1.0 && (count == 2 || count == 3))) {*/
    /*gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); //cell lives...*/
  /*} else {*/
    /*gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0); //cell dies...*/
  /*}*/
}

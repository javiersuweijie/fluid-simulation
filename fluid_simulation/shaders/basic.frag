uniform vec2 u_resolution;
uniform float u_time;
void main() {
  vec2 st = gl_FragCoord.xy/u_resolution;
  float time_dep = sin(u_time);
  gl_FragData[0] = vec4(st.x, time_dep, 1.0, 1.0);
}

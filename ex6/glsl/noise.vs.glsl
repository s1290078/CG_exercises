attribute vec4 vPosition; 
attribute vec4 vNormal;

varying vec4 oNormal;
varying vec4 oPosition;

uniform mat4 MVMatrix; // Model view matrix M 
uniform mat4 NormalMatrix; // (M^{-1})^T
uniform mat4 PMatrix; // Projection matrix


void main()
{
  // vertex position in clip space
  gl_Position = PMatrix * MVMatrix * vPosition;

  // normal in eye space
  vec4 N = NormalMatrix * vNormal;
  N.w = 0.0;
  oNormal = normalize(N);

  // vertex in eye space
  oPosition = MVMatrix * vPosition;
}

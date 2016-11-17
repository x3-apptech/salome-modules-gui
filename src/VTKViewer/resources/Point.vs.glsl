#version 130
#if (__VERSION__ >= 130)
  in vec4 Color;
  in vec3 Vertex;
  in float Diameter;
#else
  #define Vertex gl_Vertex
  #define Color gl_Color;
  attribute float Diameter;
#endif

uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewProjectionMatrix;
uniform int  uGeneralPointSize;

out vec4 VSColor;
void main()
{
  gl_Position  = uModelViewProjectionMatrix * vec4 (Vertex.xyz, 1.0);

  if (uGeneralPointSize == -1)
    gl_PointSize = 700 * uProjectionMatrix[1].y * Diameter;
  else
    gl_PointSize = uGeneralPointSize;

  VSColor      = Color;
}

#version 330 core
#if (__VERSION__ >= 130)
  in vec4 ColorBG;
  in vec4 Color;
  in vec3 Vertex;
#else
  #define Vertex gl_Vertex;
  #define Color gl_Color;
#endif

out vec4[2] VSColor;
void main()
{
  gl_Position  = vec4 (Vertex.xyz, 1.0);
  VSColor[0]   = Color;
  VSColor[1]   = vec4 (ColorBG.xyz, 1.0);
}
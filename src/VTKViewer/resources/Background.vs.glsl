#version 330 core
#if (__VERSION__ >= 130)
  in vec4 Color;
  in vec3 Vertex;
#else
  #define Vertex gl_Vertex
  #define Color gl_Color;
#endif

out vec4 VSColor;
void main()
{
  gl_Position  = vec4 (Vertex.xyz, 1.0);
  VSColor      = Color;
}

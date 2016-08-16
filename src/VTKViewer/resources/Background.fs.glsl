#version 130

#if (__VERSION__ >= 130)
  out vec4 outColor;
  #define Texture2D texture
#else
  #define outColor gl_FragColor
  #define Texture2D texture2D
#endif

uniform int       uUseTexture;
uniform sampler2D uBackgroundTexture;

in vec4 VSColor;
void main()
{
  if (uUseTexture == 1)
    outColor = Texture2D(uBackgroundTexture, VSColor.xy);
  else
    outColor = VSColor;
}

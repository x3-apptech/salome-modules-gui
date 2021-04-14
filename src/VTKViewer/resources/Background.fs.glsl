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

in vec4[2] VSColor;
void main()
{
  if (uUseTexture == 1)
  {
    outColor = Texture2D(uBackgroundTexture, VSColor[0].xy);
    outColor = (outColor * outColor.a) + (VSColor[1] * (1 - outColor.a));
    outColor.a = 1.0;
  }
  else
    outColor = VSColor[0];
}
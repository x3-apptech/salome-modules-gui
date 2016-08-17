#version 130

#if (__VERSION__ >= 130)
  out vec4 outColor;
  #define Texture2D texture
#else
  #define outColor gl_FragColor
  #define Texture2D texture2D
#endif

uniform sampler2D uPointSprite;

in vec4 VSColor;
void main()
{
  vec4 aColor = Texture2D(uPointSprite, gl_PointCoord) * VSColor;
  if (aColor.a < 0.5)
    discard;

  outColor = aColor;
}

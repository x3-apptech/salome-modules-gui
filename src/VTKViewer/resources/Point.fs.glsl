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
  outColor = VSColor;
  vec4 testColor = Texture2D(uPointSprite, gl_PointCoord);
  if (testColor.r > 0.1)
    outColor = testColor * outColor;
  else
    discard;
}

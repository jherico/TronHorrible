uniform sampler2D Texture0;

varying vec4 oColor;
varying vec2 oTexCoord;

void main()
{
   gl_FragColor = oColor * texture2D(Texture0, oTexCoord);
   if (gl_FragColor.a < 0.4)
       discard;
}

uniform mat4 Proj;
uniform mat4 View;

attribute vec4 Position;
attribute vec4 Color;
attribute vec2 TexCoord;
attribute vec2 TexCoord1;
attribute vec3 Normal;

varying  vec4 oColor;
varying  vec2 oTexCoord;
varying  vec2 oTexCoord1;
varying  vec3 oNormal;
varying  vec3 oVPos;

void main()
{
   gl_Position = Proj * (View * Position);
   oNormal = vec3(View * vec4(Normal,0));
   oVPos = vec3(View * Position);
   oTexCoord = TexCoord;
   oTexCoord1 = TexCoord1;
   oColor = Color;
}

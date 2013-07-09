uniform mat4 View;

attribute vec4 Position;
attribute vec4 Color;
attribute vec2 TexCoord;
attribute vec3 Normal;

varying  vec4 oColor;
varying  vec2 oTexCoord;
varying  vec3 oNormal;

void main()
{
   gl_Position = View * Position;
   oTexCoord = TexCoord;
   oColor = Color;
   oNormal = vec3(View * vec4(Normal,0));
}
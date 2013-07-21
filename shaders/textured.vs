attribute vec4 in_position;
attribute vec4 in_texture;

varying vec4 v_texCoord;

void main()
{
   v_texCoord = in_texture;
   gl_Position = in_position;
}

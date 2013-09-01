attribute vec4 in_position;
attribute vec4 in_texture;

varying vec4 v_texture;

void main()
{
   gl_Position = in_position;
   v_texture = in_texture;
}

attribute vec4 in_position;
attribute vec4 in_texture;

varying vec4 v_texture;

void main()
{
    v_texture = in_texture;
    gl_Position = in_position;
}

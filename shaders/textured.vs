uniform mat4 u_modelview;
uniform mat4 u_projection;

attribute vec4 in_position;
attribute vec4 in_texture;
attribute vec4 in_normal;
attribute vec4 in_color;

varying vec4 v_color;
varying vec4 v_texture;
varying vec4 v_normal;

void main()
{
    v_normal = in_normal;
    v_texture = in_texture;
    v_color = in_color;

    gl_Position = in_position;
}

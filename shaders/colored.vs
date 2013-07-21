uniform mat4 u_modelview;
uniform mat4 u_projection;

attribute vec4 in_position;
attribute vec4 in_color;

varying vec4 v_color;

void main() {
	v_color = in_color;
	gl_Position   = u_projection * u_modelview * in_position;
}


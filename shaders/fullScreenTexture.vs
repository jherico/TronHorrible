attribute vec4 in_position;
attribute vec4 in_texture;
uniform float in_sourceRatio;
uniform float in_targetRatio;

varying vec4 v_texCoord;

void main() {
    v_texCoord = in_texture;
    gl_Position = in_position - 0.0;
    if (in_sourceRatio != in_targetRatio) {
        float ratioRatio = in_sourceRatio / in_targetRatio;
        if (ratioRatio > 1.0) {
            gl_Position.y /= ratioRatio;
        } else {
            gl_Position.x *= ratioRatio;
        }
    }
}

precision mediump float;

varying vec4 v_texture;
uniform sampler2D s_texture;

const float width = 1280.0;
const float eyeWidth = 640.0;
const float height = 800.0;


void main() {
    vec2 tex = v_texture.xy;
    tex -= 0.5;
    tex *= 2.0;
    tex.y /= width / height;
    float rsq = length(tex);
    // No aspect correction, source is
    float x = v_texture.x;
    if (x >= 0.5) {
        x -= 0.5;
    } else {
    }
    x *= 2.0;
    //gl_FragColor = texture2D(s_texture, v_texture.xy);

    // gl_FragColor = texture2D(s_texture, v_texture.xy);
     gl_FragColor = vec4(0, 0, rsq, 1);
    // gl_FragColor = vec4(v_texture.y, 0, 0, 1);
}

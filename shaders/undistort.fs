precision mediump float;

varying vec4 v_texCoord;
uniform sampler2D s_texture;

const float f = 0.8;
const float PI = 3.14159;

void main() {
    vec2 source = v_texCoord.xy;
    source *= 2.0;  source -= 1.0;
    float phi = atan(source.y,source.x);
    float R_d = length(source);
    float r = atan(R_d,f) / PI;
    vec2 target = vec2(cos(phi), sin(phi));
    target *= r;
    target *= 1.2;
    target += 0.5;
    gl_FragColor = texture2D(s_texture, target);
}

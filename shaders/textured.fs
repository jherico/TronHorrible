precision mediump float;

varying vec4 v_texture;
uniform sampler2D s_texture;

void main() {
    gl_FragColor = texture2D(s_texture, v_texture.xy);
    //gl_FragColor = vec4(v_texture.y, 0, 0, 1);
}

precision mediump float;

varying vec4 v_texCoord;
uniform sampler2D s_texture;

void main() {
    gl_FragColor = texture2D(s_texture, v_texCoord.xy);
    //gl_FragColor = vec4(v_texCoord.xyz, 1);
}

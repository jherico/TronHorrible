precision mediump float;

varying vec4 v_texture;
uniform sampler2D s_texture;

const float width = 1280.0;
const float eyeWidth = 640.0;
const float height = 800.0;
const float aspect = eyeWidth / height;
const float horizontalScreenSize = 0.14976;
const float lensSeperationDistance = 0.0635;
const float eyeToScreenDistance = 0.041;
const float lensCenterOffset = 0.151976421;
const float fillScale = 1.44139099;

//        HMD.HScreenSize            = ;
//        HMD.VScreenSize            = HMD.HScreenSize / (1280.0f / 800.0f);
//        HMD.InterpupillaryDistance = InterpupillaryDistance;
//        HMD.LensSeparationDistance = f;
//        HMD.EyeToScreenDistance    = 0.041f;


vec2 textureCoordsToDistortionOffsetCoords(vec2 texCoord) {
    bool left = (texCoord.x < 0.5);
    vec2 result = texCoord;
    result.y = 1.0 - result.y;
    if (result.x > 0.5) {
        result.x -= 0.5;
    }
    result *= vec2(4.0, 2.0);
    result -= 1.0;
    result.y /= aspect;
    if (left) {
        result.x -= lensCenterOffset;
    } else {
        result.x += lensCenterOffset;
    }
    result.x /= 1280.0 / 720.0;
    result *= 1.35;
    result += 1.0;
    result /= 2.0;

//    // Convert from using the center of the screen as the origin to
//    // using the lens center as the origin
//    result -= u_lensCenterOffset;
//
//    // Correct for the aspect ratio
//    result.y /= aspect;
//
    return result;
}

void main() {
//    gl_FragColor = vec4(v_texture.xy, 1, 1);
//    return;

    vec2 tex = textureCoordsToDistortionOffsetCoords(v_texture.xy);;
    float rsq = length(tex);
    if (tex.y > 1.0) {
        discard; // gl_FragColor = vec4(0, 0, 0, 1);
    } else  if (tex.y < 0.0) {
        discard; // gl_FragColor = vec4(0, 0, 0, 1);
    } else {
        gl_FragColor = vec4(tex.xy, 1, 1);
//        gl_FragColor = texture2D(s_texture, tex).bgra;
    }
}

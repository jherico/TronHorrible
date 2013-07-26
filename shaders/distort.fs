precision mediump float;

uniform float AspectRatio;
uniform vec2 LensCenter;
uniform vec4 HmdWarpParam;

uniform sampler2D s_texture;
varying vec4 v_texCoord;
const vec2 scaleIn = vec2(.62, .62);

vec4 HmdWarp(vec2 in_) {
    // Theta is now a vector from the center of the lens to the coordinate
    vec2 theta = (in_ - LensCenter) * scaleIn;
    theta.y /= AspectRatio;
    theta *= 1.5;
    // Find the distance squared between the center of the lens
    // and the incoming texture coordinate.
    float rSq = theta.x * theta.x + theta.y * theta.y;

    // Find the factor by which to pull from the texture
    float factor =
            HmdWarpParam.x +
            HmdWarpParam.y * rSq +
            HmdWarpParam.z * rSq * rSq +
            HmdWarpParam.w * rSq * rSq * rSq;

    theta *= factor;
    theta.y *= AspectRatio;
//    theta *= ScaleFactor;
    theta += LensCenter;

    return vec4(theta, rSq, factor);
}

void main() {
    gl_FragColor = texture2D(s_texture, v_texCoord.xy);
    return;
//    gl_FragColor = vec4(v_texCoord.xy, 0, 1);
    vec4 warp = HmdWarp(v_texCoord.xy);
    gl_FragColor = vec4(warp.b, 0, 0, 1);
    //return;
    vec2 tc = warp.xy;
    if (!all(equal(clamp(tc, vec2(0), vec2(1)), tc)))
        gl_FragColor = vec4(warp);
    else
        gl_FragColor = texture2D(s_texture, tc.xy);

//    gl_FragColor = vec4(0);
//    gl_FragColor.r = length(abs(v_texCoord.xy - LensCenter));
//    gl_FragColor.g = length(abs(v_texCoord.xy - ScreenCenter));
//    gl_FragColor.b = length(tc);
//    gl_FragColor.g = tc.x;
//    gl_FragColor.r = (warp.w - 1.0);
//    gl_FragColor.r =     HmdWarpParam.x;
//        gl_FragColor.b = v_texCoord.x;
//        gl_FragColor.g = v_texCoord.y;
//    gl_FragColor = vec4(, 0, 1);
//    gl_FragColor = vec4(abs(v_texCoord.xy - ScreenCenter), 0, 1);

//    gl_FragColor = texture2D(s_texture, v_texCoord.xy);
  //  gl_FragColor = vec4(tc.x, 0, tc.y, 1);
}


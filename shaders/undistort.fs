precision mediump float;

//# Pertinent equations:
//# note phi=fov/2; fov=field of view (aperture)
//# note r=N/2; N=min(width,height)
//# perspective: r=f*tan(phi); f=r/tan(phi); f=(N/2)/tan((fov/2)*(pi/180))=N/(2*tan(fov*pi/360))
//# linear: r=f*phi; f=r/phi; f=(N/2)/((fov/2)*(pi/180))=N*180/(fov*pi)
//# equalarea: r=2*f*sin(phi/2); f=(r/2)/sin(phi/2); f=(N/4)/(sin((fov/4)*(pi/180)))=N/(4*sin(fov*pi/720))
//# orthographic: r=f*sin(phi); f=r/sin(phi); f=(N/2)/sin((fov/2)*(pi/180))=N/(2*sin(fov*pi/360))
//# stereographic: r=2*f*tan(phi/2); f=(r/2)/tan(phi/2); f=(N/4)/(tan((fov/4)*(pi/180)))=N/(4*tan(fov*pi/720))

varying vec4 v_texCoord;
uniform sampler2D s_texture;

const float f = 0.7;
const float PI = 3.14159;

void main() {
    vec2 source = v_texCoord.xy;
    source *= 2.0;  source -= 1.0;
    float R_u = length(source);
    // R_u = f*tan(theta)
    float theta = atan(R_u/f);
    // Equisolid
    // float R_d = 2.0 * f * sin(theta/2.0);
    // Ortho
    // float R_d = f * sin(theta);
    // Stereographic
    float R_d = 2.0 * f * tan(theta/2.0);
    float ratio = R_d / R_u;

//    source *= ratio;
//    float phi = atan(source.y,source.x);
//    float r = atan(R_d,f) / PI;
//    float R_u = f*tan(2*asin(R_d/(2*f)))
    gl_FragColor = vec4(0, 0, ratio, 1.0);
//    gl_FragColor = vec4(theta , 0, 0, 1.0);
    vec2 target = source * ratio;
    source += 1.0;
    source /= 2.0;
    target += 1.0;
    target /= 2.0;
//    target += 0.5;
    gl_FragColor = texture2D(s_texture, target);
//    gl_FragColor = texture2D(s_texture, source);
}

//const float f = 0.8;
//const float PI = 3.14159;
//
//void main() {
//    vec2 source = v_texCoord.xy;
//    source *= 2.0;  source -= 1.0;
//    float phi = atan(source.y,source.x);
//    float R_d = length(source);
//    float r = atan(R_d,f) / PI;
//    vec2 target = vec2(cos(phi), sin(phi));
//    target *= r;
//    target *= 1.2;
//    target += 0.5;
//    gl_FragColor = texture2D(s_texture, target);
//}


attribute vec4 in_position;
attribute vec4 in_texture;

varying vec4 v_texCoord;

const float sourceAspect = 1280.0/720.0;
const float targetAspect = 1280.0/800.0;
const float aspectChange = targetAspect / sourceAspect;


void main()
{
   v_texCoord = in_texture;
   gl_Position = in_position;
   if (aspectChange > 1.0) {
       gl_Position.x /= aspectChange;
   } else {
       gl_Position.y *= aspectChange;
   }
}

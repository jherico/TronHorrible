attribute vec4 a_position;
attribute vec4 a_texCoord;

varying vec4 v_texCoord;
void main()
{
   v_texCoord = a_texCoord;
   gl_Position = a_position;
}


/*
//uniform mat4 u_mvpMatrix;


void main()
{
//  gl_Position = u_mvpMatrix * a_position;
   gl_Position = a_position;
};


*/

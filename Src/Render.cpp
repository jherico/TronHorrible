#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>
GLuint vertexBuffer = -1;
GLuint indexBuffer = -1;

GLuint posAttr;
GLuint texAttr;

#define PH 0.5f
#define PQ 0.25f
#define NH -0.5f
#define NO -1.0f
#define ON 1.0f
#define ZE 0.0f

static GLfloat vertices[] = { //
        NO, NO, ZE, ON, ZE, ZE, ZE, ON, //
        ON, NO, ZE, ON, ON, ZE, ZE, ON, //
        ON, ON, ZE, ON, ON, ON, ZE, ON, //
        NO, ON, ZE, ON, ZE, ON, ZE, ON //
        };
static GLuint indices[] = { 0, 1, 2, 2, 3, 0, };

void init_gl(GLint ploc, GLint tloc) {
    posAttr = ploc;
    texAttr = tloc;
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 32 * 4, vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(posAttr);
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 32, (void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void render_gl() {
    static GLfloat vVertices[] = { //
            -1, -1, 0, //
            1, -1, 0, //
            1, 1, 0, //
            -1, 1, 0  //
            };
    static GLfloat vTex[] = { //
            0, 0, //
            1, 0, //
            1, 1, //
            0, 1, //
    };

    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(posAttr);
    glVertexAttribPointer(texAttr, 2, GL_FLOAT, GL_FALSE, 0, vTex);
    glEnableVertexAttribArray(texAttr);
    glDrawArrays( GL_TRIANGLE_FAN, 0, 4);

    //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*) 0);

}

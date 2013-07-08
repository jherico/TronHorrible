#include "common.hpp"
#include <string.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

string slurp(const std::string& in) {
    ifstream ins;
    ins.open(in.c_str());
    return slurp(ins);
}

string slurp(ifstream& in) {
    stringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

namespace GL {
namespace ES {

static char TEMP_BUFFER[8192];

Shader::Shader(GLenum type, const std::string & sourceFile) :
        type(type), shader(0), sourceFile(sourceFile) {
    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0)
        throw new std::string("Failed to allocate shader id");

    {
        std::string shaderSrc = slurp(sourceFile);
        strcpy(TEMP_BUFFER, shaderSrc.c_str());
        const char * cbuffer = TEMP_BUFFER;
        const char * const * bufferPtr = &cbuffer;
        // Load the shader source
        glShaderSource(shader, 1, bufferPtr, NULL);
    }

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        throw new string(getLog());
    }
}

Shader::~Shader() {
    glDeleteShader(shader);
}

std::string Shader::getLog() {
    string log;
    GLint infoLen = 0;
    glGetShaderInfoLog(shader, 8192, &infoLen, TEMP_BUFFER);
    log = string(TEMP_BUFFER);
    return log;
}


Program::Program(const std::string & vss, const std::string & fss) :
        vs(GL_VERTEX_SHADER, vss), fs(GL_FRAGMENT_SHADER, fss), program(0) {
    // Create the program object
    program = glCreateProgram();
    if (program == 0)
        throw new string("Failed to allocate GL program");

    glAttachShader(program, vs.shader);
    glAttachShader(program, fs.shader);

    // Bind vPosition to attribute 0
    glBindAttribLocation(program, 0, "vPosition");

    // Link the program
    glLinkProgram (program);

    // Check the link status
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked) {
        throw new string(getLog());
    }
}

Program::~Program() {
    glDeleteProgram (program);
}

std::string Program::getLog() {
    string log;
    GLint infoLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);

    if (infoLen > 1) {
        char* infoLog = new char[infoLen];
        glGetProgramInfoLog(program, infoLen, NULL, infoLog);
        log = string(infoLog);
        delete[] infoLog;
    }
    return log;
}

}
}

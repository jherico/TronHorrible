#pragma once
#include "Common.hpp"
#include <string>
#include <map>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#pragma once

enum VarType {
    UNIFORM,
    ATTRIBUTE,
};

struct Var {
    std::string name;
    GLint size;
    GLenum type;
    GLuint id;
    GLint location;
    VarType vartype;
    Var();
    Var(VarType varType, GLuint program, GLuint id);
};

class Shader {
    friend class Program;
    const GLenum type;
    GLuint shader;
    std::string sourceFile;
    time_t compiled;

public:
    Shader(GLenum type, const std::string & sourceFile);
    virtual ~Shader();
    static std::string getLog(GLuint shader);
    void compile();
    bool isStale();
};

class Program {
    Shader vs;
    Shader fs;
    GLuint program;
    std::map<std::string, Var> vars;

public:
    Program(const std::string & name);
    Program(const std::string & vss, const std::string & fss);
    virtual ~Program();
    GLint getLocation(VarType type, const std::string & name);
    void use();
    void link();
    static std::string getLog(GLuint program);
};

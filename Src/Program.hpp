#pragma once
#include "Common.hpp"

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

    Var() {}; // for map usage
    Var(VarType type, GLuint program, GLuint id) : vartype(type), id(id) {
    }
};

class Shader {
    friend class Program;
    const GLenum type;
    GLuint shader;
    const std::string sourceFile;
    boost::posix_time::ptime compiled;

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

    void setUniform1f(GLint location, GLfloat);
    void setUniform2f(GLint location, GLfloat, GLfloat);
    void setUniform4f(GLint location, GLfloat, GLfloat, GLfloat, GLfloat);
    void setUniform4x4f(GLint location, GLfloat *);
    void setUniform4x4f(GLint location, const glm::mat4 &);
    void use();
    void link();
    static std::string getLog(GLuint program);
};

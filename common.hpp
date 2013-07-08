#pragma once
#include <string>
#include <strstream>
#include <fstream>
#include <sstream>
#include <GLES2/gl2.h>

std::string slurp(const std::string& in);
std::string slurp(std::ifstream& in);

namespace GL {
namespace ES {

class Shader {
public:
    GLenum type;
    GLuint shader;
    std::string sourceFile;

    Shader(GLenum type, const std::string & sourceFile);
    virtual ~Shader();
    std::string getLog();
};

class Program {
public:
    Shader vs;
    Shader fs;
    GLuint program;
    Program(const std::string & vss, const std::string & fss);
    virtual ~Program();
    std::string getLog();
};

}
}

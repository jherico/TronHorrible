#include "Program.hpp"

using namespace std;
using namespace boost::posix_time;
using namespace boost::filesystem;

static char TEMP_BUFFER[8192];

Shader::Shader(GLenum type, const std::string & sourceFile) :
        type(type), shader(0), sourceFile(sourceFile), compiled(neg_infin) { }

Shader::~Shader() {
    glDeleteShader(shader);
}

std::string Shader::getLog(GLuint shader) {
    string log;
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

    if (infoLen > 1) {
        char* infoLog = new char[infoLen];
        glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
        log = string(infoLog);
        delete[] infoLog;
    }
    return log;
}

bool Shader::isStale() {
    ptime filetime = from_time_t(last_write_time(sourceFile));
    return (filetime > compiled);
}

void Shader::compile() {
    compiled = from_time_t(last_write_time(sourceFile));


    // Create the shader object
    GLuint newShader = glCreateShader(type);
    if (newShader == 0)
        throw string("fuck");

    {
        std::string shaderSrc = slurp(sourceFile);
        strcpy(TEMP_BUFFER, shaderSrc.c_str());
        const char * cbuffer = TEMP_BUFFER;
        const char ** bufferPtr = &cbuffer;
        // Load the shader source
        glShaderSource(newShader, 1, bufferPtr, NULL);
    }

    // Compile the shader
    glCompileShader(newShader);

    // Check the compile status
    GLint compiled;
    glGetShaderiv(newShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        cerr << "Failed to compile shader " << sourceFile << endl;
        cerr << "Error log was: " << getLog(newShader);
//                    throw gl_exception() << gl_error_log(getLog()) << boost::errinfo_file_name(sourceFile);
        return;
    }

    if (0 != shader) {
        glDeleteShader(shader);
    }
    shader = newShader;
}


Program::Program(const std::string & vss, const std::string & fss)
        : vs(GL_VERTEX_SHADER, vss), fs(GL_FRAGMENT_SHADER, fss), program(0) {
}

GLint Program::getLocation(VarType varType, const std::string & name) {
    auto itr = vars.find(name);
    if (itr == vars.end()) {
        return -1;
    }
    const Var & var = itr->second;
    if (var.vartype != varType) {
        return -1;
    }
    return var.location;
}

Program::~Program() {
    glDeleteProgram(program);
}

std::string Program::getLog(GLuint program) {
    std::string log;
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

void Program::link() {
    if (0 == program || vs.isStale() || fs.isStale()) {
        if (vs.isStale()) {
            vs.compile();
        }

        if (fs.isStale()) {
            fs.compile();
        }

        if (0 != program) {
            glDeleteProgram(program);
            program = 0;
        }

        // Create the program object
        program = glCreateProgram();
        if (program == 0)
            throw new string("Failed to allocate GL program");

        glAttachShader(program, vs.shader);
        glAttachShader(program, fs.shader);

        // Bind vPosition to attribute 0
        glBindAttribLocation(program, 0, "vPosition");

        // Link the newProgram
        glLinkProgram(program);

        // Check the link status
        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);

        if (!linked) {
            throw string(getLog(program));
        }

        int numVars;
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numVars);
        for (int i = 0; i < numVars; ++i) {
            Var var(ATTRIBUTE, program, i);
            cerr << "Found attribute " << var.name << endl;
            vars[var.name] = var;
        }
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numVars);
        for (int i = 0; i < numVars; ++i) {
            Var var(UNIFORM, program, i);
            cerr << "Found uniform " << var.name << endl;
            vars[var.name] = var;
        }
    }
}

void Program::use() {
    link();
    glUseProgram(program);
}


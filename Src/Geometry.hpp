#pragma once
#include "Common.hpp"


enum VertexFlags {
    VERTEX_HAS_TEX = 0x01,
    VERTEX_HAS_NORM = 0x02,
    VERTEX_HAS_COLOR = 0x04,
};

struct Vertex {
    glm::vec4 pos;
    glm::vec4 tex;
    glm::vec4 nor;
    glm::vec4 col;

    Vertex(const glm::vec4 & pos,
            const glm::vec4 & tex = glm::vec4(),
            const glm::vec4 & nor = glm::vec4(),
            const glm::vec4 & col = glm::vec4());
    Vertex(const glm::vec3 & pos,
            const glm::vec2 & tex = glm::vec2(),
            const glm::vec3 & nor = glm::vec3(),
            const glm::vec3 & col = glm::vec3());

    static Vertex simpleTextured(const glm::vec3 & pos, const glm::vec2 & tex);
    static Vertex simpleColored(const glm::vec3 & pos, const glm::vec4 & col);
    static Vertex simpleColored(const glm::vec3 & pos, const glm::vec3 & col);
};

struct VertexBuffer {
    typedef glm::vec4 V;
    typedef std::vector<V> VV;
    GLuint vertexFlags;
    VV vertices;
    GLuint buffer;

    VertexBuffer();
    VertexBuffer(GLuint vertexFlags, std::initializer_list<Vertex> vertices);
    VertexBuffer(GLuint vertexFlags, std::vector<Vertex> vertices);

    template <class T> void setVertices(const T & vertices_in) {
        typename T::const_iterator end = vertices_in.end();

        for (typename T::const_iterator itr = vertices_in.begin(); itr != end; ++itr ) {
            const Vertex & vertex = *itr;
            vertices.push_back(vertex.pos);
            if (vertexFlags & VERTEX_HAS_TEX) {
                vertices.push_back(vertex.tex);
            }
            if (vertexFlags & VERTEX_HAS_NORM) {
                vertices.push_back(vertex.nor);
            }
            if (vertexFlags & VERTEX_HAS_COLOR) {
                vertices.push_back(vertex.col);
            }
        }
        initBuffer();
    }


    void bind();
private:
    void initBuffer();
};

struct IndexBuffer {
    typedef std::vector<GLuint> VI;
    GLenum type;
    VI indices;
    GLuint buffer;
    IndexBuffer();
    IndexBuffer(GLenum type, std::initializer_list<GLuint> indices);
    IndexBuffer(GLenum type, std::vector<GLuint> indices);

    template <class T> void setIndices(const T & indices_in) {
        indices.clear();
        indices.assign(indices_in.begin(), indices_in.end());
        initBuffer();
    }

    void initBuffer();
    void bind();
    void render();
};

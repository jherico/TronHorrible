#include "Geometry.hpp"

GLuint getVertexSize(GLuint vertexFlags) {
    int vertexSize = 1;
    if (vertexFlags & VERTEX_HAS_TEX) {
        ++vertexSize;
    }
    if (vertexFlags & VERTEX_HAS_NORM) {
        ++vertexSize;
    }
    if (vertexFlags & VERTEX_HAS_COLOR) {
        ++vertexSize;
    }
    return vertexSize;
}

VertexBuffer::VertexBuffer(GLuint vertexFlags, std::vector<Vertex> vertices_in)
        : vertexFlags(vertexFlags) {
    setVertices(vertices_in);
}

VertexBuffer::VertexBuffer() {

}

VertexBuffer::VertexBuffer(GLuint vertexFlags, std::initializer_list<Vertex> vertices_in)
        : vertexFlags(vertexFlags) {
    setVertices(vertices_in);
}

void VertexBuffer::initBuffer() {
    int vertexSize = getVertexSize(vertexFlags);
    int vertexCount = vertices.size() / vertexSize;
    GLuint vertexStride = vertexSize * sizeof(V);
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, vertexStride * vertexCount, &this->vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

IndexBuffer::IndexBuffer() {
}

IndexBuffer::IndexBuffer(GLenum type, std::initializer_list<GLuint> indices)
        : type(type), indices(indices.begin(), indices.end()) {
    setIndices(indices);
}

IndexBuffer::IndexBuffer(GLenum type, std::vector<GLuint> indices)
        : type(type), indices(indices.begin(), indices.end()) {
    setIndices(indices);
}

void IndexBuffer::initBuffer() {
    glGenBuffers(1, &buffer);
    GLuint size = this->indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * size, &this->indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

void IndexBuffer::render() {
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    int size = indices.size();
    glDrawElements(type, size, GL_UNSIGNED_INT, (void*) 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

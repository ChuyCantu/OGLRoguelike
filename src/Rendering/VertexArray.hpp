#ifndef __VERTEXARRAY_H__
#define __VERTEXARRAY_H__

#include <glad/glad.h>

struct VertexLayout {
    bool normalized{false};
};

class VertexArray {
   public:
    // Vertex layout should be in order of the enum
    enum class Layout {
        Pos = 1,
        Color = 2,
        UV = 4,
        Normal = 8,
        Tangent = 16,
        Bitangent = 32,
        LAYOUT_MAX
    };

   private:
    GLuint VAO{0};
    GLuint VBO{0};
    GLuint EBO{0};
    unsigned int numVerts{0};
    unsigned int numIndices{0};

   public:
    // For legacy version
    VertexArray(const void* vertices, unsigned int verticesCount,
                const unsigned int* indices, unsigned int indicesCount);
    VertexArray(const void* vertices, unsigned int verticesCount,
                const unsigned int* indices, unsigned int indicesCount, Layout layout);
    ~VertexArray();

    void Use();

    GLuint GetID() const { return VAO; }
    unsigned int GetVerticesCount() const { return numVerts; }
    unsigned int GetIndicesCount() const { return numIndices; }
};

inline VertexArray::Layout operator|(VertexArray::Layout a, VertexArray::Layout b) {
    return static_cast<VertexArray::Layout>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(VertexArray::Layout a, VertexArray::Layout b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

#endif  // __VERTEXARRAY_H__
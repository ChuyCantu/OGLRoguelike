#include "VertexArray.hpp"

VertexArray::VertexArray(const void* vertices, unsigned int verticesCount,
                         const unsigned int* indices, unsigned int indicesCount) {
    // Create Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int fpv{5};  // floats per vertex

    // Create Vertex Buffer Object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesCount * fpv * sizeof(float), vertices, GL_STATIC_DRAW);

    // Create Element Buffer Object
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, fpv * sizeof(float), (void*)0);

    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, fpv * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, fpv * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

    // Unbind VAO now that we configured what was needed
    glBindVertexArray(0);
}

VertexArray::VertexArray(const void* vertices, unsigned int verticesCount,
                         const unsigned int* indices, unsigned int indicesCount, Layout layout) {
    numVerts = verticesCount;
    numIndices = indicesCount;

    // Create Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int fpv{0};  // floats per vertex
    if (layout & VertexArray::Layout::Pos) fpv += 3;
    if (layout & VertexArray::Layout::Color) fpv += 3;
    if (layout & VertexArray::Layout::UV) fpv += 2;
    if (layout & VertexArray::Layout::Normal) fpv += 3;
    if (layout & VertexArray::Layout::Tangent) fpv += 3;
    if (layout & VertexArray::Layout::Bitangent) fpv += 3;

    // Create Vertex Buffer Object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesCount * fpv * sizeof(float), vertices, GL_STATIC_DRAW);

    // Create Element Buffer Object
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    int attribNum{0};
    unsigned long long offset{0};
    if (layout & VertexArray::Layout::Pos) {
        glEnableVertexAttribArray(attribNum);
        glVertexAttribPointer(attribNum, 3, GL_FLOAT, GL_FALSE, fpv * sizeof(float), reinterpret_cast<void*>(offset));
        ++attribNum;
        offset += 3;
    }
    if (layout & VertexArray::Layout::Color) {
        glEnableVertexAttribArray(attribNum);
        glVertexAttribPointer(attribNum, 3, GL_FLOAT, GL_FALSE, fpv * sizeof(float), reinterpret_cast<void*>(offset * sizeof(float)));
        ++attribNum;
        offset += 3;
    }
    if (layout & VertexArray::Layout::UV) {
        glEnableVertexAttribArray(attribNum);
        glVertexAttribPointer(attribNum, 2, GL_FLOAT, GL_FALSE, fpv * sizeof(float), reinterpret_cast<void*>(offset * sizeof(float)));
        ++attribNum;
        offset += 2;
    }
    if (layout & VertexArray::Layout::Normal) {
        glEnableVertexAttribArray(attribNum);  // (void*)offsetof(Vertex, normal) if using Vertex struct in Mesh.hpp
        glVertexAttribPointer(attribNum, 3, GL_FLOAT, GL_FALSE, fpv * sizeof(float), reinterpret_cast<void*>(offset * sizeof(float)));
        ++attribNum;
        offset += 3;
    }
    if (layout & VertexArray::Layout::Tangent) {
        glEnableVertexAttribArray(attribNum);
        glVertexAttribPointer(attribNum, 3, GL_FLOAT, GL_FALSE, fpv * sizeof(float), reinterpret_cast<void*>(offset * sizeof(float)));
        ++attribNum;
        offset += 3;
    }
    if (layout & VertexArray::Layout::Bitangent) {
        glEnableVertexAttribArray(attribNum);
        glVertexAttribPointer(attribNum, 3, GL_FLOAT, GL_FALSE, fpv * sizeof(float), reinterpret_cast<void*>(offset * sizeof(float)));
        ++attribNum;
        offset += 3;
    }

    // Unbind VAO now that we configured what was needed
    glBindVertexArray(0);
}

VertexArray::~VertexArray() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void VertexArray::Use() {
    glBindVertexArray(VAO);
}

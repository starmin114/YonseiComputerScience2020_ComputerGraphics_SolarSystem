
// Plane has 4 vertices
// # of elements in vertex array = 4 * 3 (xyz) = 12
//    v0  v3
//    v1  v2
// Vertex shader: the location (0: position attrib (vec3), 1: normal attrib (vec3),
//                              2: color attrib (vec4), and 3: texture coordinate attrib (vec2))
// Fragment shader: should catch the vertex color from the vertex shader

#ifndef PLANE_H
#define PLANE_H

#include "shader.h"

class Plane {
public:
    
    // vertex position array
    GLfloat vertices[12]  = { // edge size = 1, 12 elements
        -.5f, .5f, .0f,   -.5f, -.5f, .0f,    .5f, -.5f, .0f,    .5f, .5f, .0f
    };
    
    // normal array
    GLfloat normals[12] = {  // 12 elements
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1
    };
    
    // colour array
    GLfloat colors[16] = { // initialized as RGBA, 12 elements
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1
    };
    
    // texture coord array
    GLfloat texCoords[8] = {
        0, 1,
        0, 0,
        1, 0,
        1, 1
    };
    
    // index array for glDrawElements()
    GLuint indicies[6] = { // 6 indicies
        0, 1, 2,
        2, 3, 0
    };

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    
    unsigned int vSize = sizeof(vertices);
    unsigned int nSize = sizeof(normals);
    unsigned int cSize = sizeof(colors);
    unsigned int tSize = sizeof(texCoords);
    
    Plane() {
        initBuffers();
    };
    
    Plane(float dx, float dy, float dz, float s) {
        scale(s);
        translate(dx, dy, dz);
        initBuffers();
    };
    
    void initBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        // copy vertex attrib data to VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vSize+nSize+cSize+tSize, 0, GL_STATIC_DRAW); // reserve space
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertices);                  // copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, normals);               // copy norms after verts
        glBufferSubData(GL_ARRAY_BUFFER, vSize+nSize, cSize, colors);          // copy cols after norms
        glBufferSubData(GL_ARRAY_BUFFER, vSize+nSize+cSize, tSize, texCoords); // copy texs after cols
        
        // copy index data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
        
        // attribute position initialization
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // position attrib
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)vSize); // normal attrib
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(vSize+nSize)); //color attrib
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)(vSize+nSize+cSize)); // tex coord
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };
    
    void updateVBO() {
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertices);                  // copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, normals);               // copy norms after verts
        glBufferSubData(GL_ARRAY_BUFFER, vSize+nSize, cSize, colors);          // copy cols after norms
        glBufferSubData(GL_ARRAY_BUFFER, vSize+nSize+cSize, tSize, texCoords); // copy texs after cols
        
        // attribute position initialization
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // position attrib
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)vSize); // normal attrib
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(vSize+nSize)); //color attrib
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)(vSize+nSize+cSize)); // tex coord
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
    };
    
    void draw(Shader *shader) {
        shader->use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    };
    
    void translate(float dx, float dy, float dz) {
        for (int i = 0; i < 12; i++) {
            if (i % 3 == 0) vertices[i] += dx;
            else if (i % 3 == 1) vertices[i] += dy;
            else if (i % 3 == 2) vertices[i] += dz;
        }
    };
    
    void scale(float s) {
        for (int i = 0; i < 12; i++)
            vertices[i] *= s;
    };
};


#endif

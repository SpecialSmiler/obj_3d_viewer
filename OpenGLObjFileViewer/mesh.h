#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shader_s.h"

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    string type;
    string path; // we store the path of the texture to compare with other textures
};

class Mesh {
public:
    // mesh data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void Draw(Shader& shader, GLenum mode = GL_TRIANGLES);
    void PrintFaceIndices();

private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

void PrintIndicies(vector<unsigned int>& indices, unsigned int lineSize = 3);

#endif
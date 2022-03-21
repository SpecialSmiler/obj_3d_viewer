#ifndef WIREFRAMEMESH_H
#define WIREFRAMEMESH_H
#include "mesh.h"

using namespace std;

class WireframeMesh { //with quad and n-gon faces
public:
    // mesh data
    vector<Vertex>       vertices;
    vector<unsigned int> edgeIndices;

    WireframeMesh(vector<Vertex> vertices, vector<unsigned int> edgeIndices);
    void Draw(Shader& shader);

private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};
#endif // !WIREFRAMEMESH_H



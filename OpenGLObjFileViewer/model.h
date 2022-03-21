#ifndef MODEL_H
#define MODEL_H


#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader_s.h"
#include "mesh.h"
#include "wireframe_mesh.h"

using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
unsigned int TextureFromFile(const string& path);

class Model
{
public:
    Model(string const& path);
    void Draw(Shader& shader, GLenum mode = GL_TRIANGLES);
    void DrawMatcap(Shader& shader, unsigned int matcapId);
    void DrawWireframe(Shader& shader);
    void PrintMeshesIndices();

private:
    // model data
    vector<Texture> textures_loaded;
    vector<Mesh> meshes;//triangulated meshes
    vector<WireframeMesh> wireframeMeshes;//un-triangulated meshes
    string directory;

    void loadModel(string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void processWireframeNode(aiNode* node, const aiScene* scene);
    WireframeMesh processWireframeMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        string typeName);


};

#endif
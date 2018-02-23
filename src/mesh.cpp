#include "include/mesh.h"
#include "include/OBJ_Loader.h"

Mesh::Mesh() : Mesh(
        0,
        0,
        nullptr,
        nullptr,
        nullptr)
{
}

Mesh::Mesh(unsigned int numV, unsigned int numT, glm::vec3* verts,
        glm::vec3* norms, glm::ivec3* indices) {
    numVertices_ = numV;
    numTriangles_ = numT;
    vertices_ = verts;
    normals_ = norms;
    indices_ = indices;
}

Mesh::~Mesh() {
    if (vertices_)
        delete [] vertices_;
    if (normals_)
        delete [] normals_;
    if (indices_)
        delete [] indices_;
}

bool Mesh::LoadMesh(const std::string& fname) {
    objl::Loader Loader;
    bool loaded = Loader.LoadFile(fname);
    if (!loaded) {
        std::cout << "Failed to open/load file: " << fname << std::endl;
        return false;
    }
    objl::Mesh m = Loader.LoadedMeshes[0];
    numVertices_ = m.Vertices.size();
    vertices_ = new glm::vec3[numVertices_];
    normals_  = new glm::vec3[numVertices_];
    for (int i = 0; i < numVertices_; i++) {
        float x,y,z;
        x = m.Vertices[i].Position.X;
        y = m.Vertices[i].Position.Y;
        z = m.Vertices[i].Position.Z;
        vertices_[i] = glm::vec3(x, y, z);
        x = m.Vertices[i].Normal.X;
        y = m.Vertices[i].Normal.Y;
        z = m.Vertices[i].Normal.Z;
        normals_[i] = glm::vec3(x, y, z);
    }
    numTriangles_ = m.Indices.size() / 3;
    indices_ = new glm::ivec3[numTriangles_];
    int tri = 0;
    for (int i = 0; i < m.Indices.size();) {
        unsigned int x = m.Indices[i++];
        unsigned int y = m.Indices[i++];
        unsigned int z = m.Indices[i++];
        indices_[tri++] = glm::ivec3(x, y, z);
    }
    return true;
}

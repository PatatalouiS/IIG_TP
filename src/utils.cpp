
#include "utils.h"
#include <cmath>

#include <QString>
#include <sstream>
#include <QFile>
#include <QChar>
#include <QTextStream>
#include <QDebug>
#include <glm/common.hpp>
#include <glm/detail/func_geometric.hpp>
#include <fstream>
#include <regex>
#include <glm/vec3.hpp>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace glm;

namespace {

const std::regex facesSeparator("(\\s)|(/)");

}

void Utils::loadOBJFile(const std::string& path, std::vector<GLfloat>& vertices,
                                      std::vector<GLfloat>& normals,
                                      std::vector<GLuint>& indices )
{
    std::ifstream file(path);
    std::string trash;
    std::string line;

   if(!file.is_open()) {
       qDebug() << "Error when loading OBJ file " << QString::fromStdString(path);
       return;
   }

    while(getline(file, line)) {
        std::stringstream stream;

        if((line[0] == 'o') || (line[0] == '#') || (line.find("vt") != std::string::npos)) {
            line.clear();
            continue;
        }
        else if (line.find("vn") != std::string::npos) {
            stream << line;
            stream >> trash;
            GLfloat vn1, vn2, vn3;
            stream >> vn1 >> vn2 >> vn3;
            insertBack( normals, { vn1, vn2, vn3 });
        }
        else if(line[0] == 'v') {
            stream << line;
            stream >> trash;
            GLfloat x, y, z;
            stream >> x >> y >> z;
            insertBack(vertices, { x, y, z });
        }
        else if(line[0] == 'f') {
            std::sregex_token_iterator it {line.begin() + 1,line.end(), facesSeparator,-1};
            std::vector<std::string> exploded(it, std::sregex_token_iterator{});
            exploded.erase(exploded.begin());
            insertBack(indices, { unsigned(stoi(exploded[0])) -1, unsigned(stoi(exploded[3])) -1, unsigned(stoi(exploded[6]))-1 });
        }
    }
}

void Utils::loadOBJFile2(const string &fileName, vector<GLfloat>& vertices,
                        vector<GLuint>& indices, vector<GLfloat>& normals, vector<GLfloat>& coord) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        fileName,
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_GenUVCoords
    );

    if (nullptr == scene) {
      cerr << importer.GetErrorString();
      return;
    }

    auto a = scene->mMeshes[0];

    auto vertices_assimp = a->mVertices;
    auto normals_assimp = a->mNormals;
    auto faces_assimp = a->mFaces;
    auto tex = a->mTextureCoords;

    auto n_vertices = a->mNumVertices;
    auto n_faces = a->mNumFaces;

    for(unsigned int i = 0; i < n_vertices; ++i) {
        Utils::insertBack(vertices, {vertices_assimp[i].x, vertices_assimp[i].y, vertices_assimp[i].z});
        Utils::insertBack(normals, {normals_assimp[i].x, normals_assimp[i].y, normals_assimp[i].z});
        Utils::insertBack(coord, {tex[0][i].x, tex[0][i].y});
    }

    for(unsigned int i = 0; i < n_faces; ++i) {
        auto& face_indices = faces_assimp[i].mIndices;
        indices.insert(indices.end(), face_indices, face_indices + 3);
    }
}


std::string  Utils::read_shader_file (const std::string& shaderFile) {
    // no feedback is provided for stream errors / exceptions.

    std::ifstream file (shaderFile);
    if (!file) return std::string ();

    file.ignore(std::numeric_limits<std::streamsize>::max());
    auto size = file.gcount();

    if (size > 0x10000) // 64KiB sanity check for shaders:
        return std::string ();

    file.clear();
    file.seekg(0, std::ios_base::beg);

    std::stringstream sstr;
    sstr << file.rdbuf();
    file.close();

    return sstr.str();
}

Mesh Utils::makeCylinder(vec3 base, vec3 axis,
                  float radius, float length, int subdiv1, int subdiv2){

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLuint> indices;

    vec3 x = vec3(1,0,0); //orthogonal(axis);
    vec3 y = cross(axis, x);

    for(int i=0; i<subdiv2; i++)
    {
        float offset = float(i)/float(subdiv2-1);
        float offset2 = (offset-0.5)*length;
        for(int j=0; j<subdiv1; j++)
        {
            float angle = 2.*glm::pi<float>()*float(j)/float(subdiv1);
            glm::vec3 position;
            glm::vec3 normal;
            position = base+offset2*axis+radius*cos(angle)*x+radius*sin(angle)*y;
            normal = normalize(cos(angle)*x+sin(angle)*y);
            vertices.push_back(position.x); vertices.push_back(position.y); vertices.push_back(position.z);
            normals.push_back(normal.x); normals.push_back(normal.y); normals.push_back(normal.z);
        }
    }


    for(int i=0; i<subdiv2-1; i++)
    {
        for(int j=0; j<subdiv1; j++)
        {
            indices.push_back(i*subdiv1+j); indices.push_back(i*subdiv1+(j+1)%subdiv1); indices.push_back(i*subdiv1+j+subdiv1);
            indices.push_back(i*subdiv1+(j+1)%subdiv1); indices.push_back(i*subdiv1+j+subdiv1); indices.push_back(i*subdiv1+(j+1)%subdiv1+subdiv1);
        }
    }

    return Mesh(vertices, indices, normals, GL_PATCHES);
}


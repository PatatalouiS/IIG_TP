
#include "utils.h"
#include <cmath>
#include <gl.h>

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
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

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



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

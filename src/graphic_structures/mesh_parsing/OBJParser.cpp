#include "OBJParser.h"
Mesh3D* OBJParser::parse(std::string path)
{
    std::shared_ptr<Shader> sh = Shader::getShader("./resources/shaders/texture");
    LOG_DEBUG(std::string("Parsing a mesh at: ")+path);

    std::vector<glm::vec3> prevertex;
    std::unordered_map<int_fast16_t,glm::vec3> prevertexmap;
    std::vector<glm::vec2> pretexture;
    std::unordered_map<int_fast16_t,glm::vec2> pretexturemap;

    OBJParser::getVertexes(std::ifstream(path),prevertex,prevertexmap);
    Hitbox h = generateHitbox(prevertex);
    OBJParser::getUVs(std::ifstream(path),pretexture,pretexturemap);

    LOG_DEBUG("Mesh parsed with UV count: "+Logger::toString(pretexture.size())+" and Vertexes (x,y,z all count as 1 each): "+Logger::toString(prevertex.size()));

    //the vertex UV pair the face is pointing to will not be at a constant position. therefore, we need to first read all UV mappings and vertexes, then use the hashmap to O(1) access
    //the vertex and UV indexes, and generate the VBO's representation of the pair. Afterwards, we will determine the position at which face the ebo should point. EBO will be in quad format
    std::vector<GLuint> faces = std::vector<GLuint>();
    std::vector<Vertex> vertexes = std::vector<Vertex>();
    std::unordered_map<Vertex,int_fast16_t> vertexmap;

    std::string line;
    auto stream = std::ifstream(path);
    std::getline(stream,line);
    
    while (line[0]!='u')
        std::getline(stream,line);
    //ifstream loaded up to faces

    size_t i = 0;
    size_t textnum = 0;
    do {
        std::getline(stream,line);
        if (line[0]=='u')
        {
            ++textnum;
            continue;
        }

        std::istringstream str{line};
        std::string f;
        Vertex v;
        str >> f;
        //parse the face
        for (int j=0;j<4;j++)
        {
            str >> f;
            auto vec = MyUtil::splitString(f,'/');
            try {
                v.position = prevertexmap.at(std::stoi(vec[0]));
                v.UV = pretexturemap.at(std::stoi(vec[1]));
                v.index = textnum;
            } catch (...) {
                LOG_DEBUG(std::string("Failed to parse a face at ")+path+std::string(". Face: ") + line);
            }
            if (vertexmap.find(v) != vertexmap.end())
                faces.push_back(vertexmap.at(v));
            else
            {
                vertexes.push_back(v);
                faces.push_back(i);
                vertexmap.emplace(v,i++);
            }
        }
    } while (stream.peek()!=EOF);
    EBO ebo = EBO(std::move(faces));
    std::vector<GLfloat> floats = std::vector<GLfloat>();
    for (i=0;i<vertexes.size();i++)
    {
        floats.push_back(vertexes.at(i).position[0]);
        floats.push_back(vertexes.at(i).position[1]);
        floats.push_back(vertexes.at(i).position[2]);
        floats.push_back(vertexes.at(i).UV[0]);
        floats.push_back(vertexes.at(i).UV[1]);
        floats.push_back(vertexes.at(i).index);
    }
    VBO vbo = VBO(std::move(floats));
    LOG_ALL("Size of EBO, then VBO:"+std::to_string(ebo.getValues().size())+", "+std::to_string(vbo.getValues().size()));
    Mesh3D* ptr = new Mesh3D(std::move(vbo),std::move(ebo),std::move(VAO(sh)),sh,GL_QUADS,std::move(h));

    //Load references for textures into Mesh
    loadTextures(std::ifstream(changeExtension(path)),ptr);

    return ptr;
}


std::string OBJParser::changeExtension(std::string oldpath)
{
    return oldpath.substr(0,oldpath.find('.',oldpath.size()-5)) + std::string(".mtl");
}
void OBJParser::loadTextures(std::ifstream stream, Mesh* ptr)
{
    LOG_ALL("Parsing Textures");
    std::string line;
    do {
        std::getline(stream,line);
        std::istringstream str(line);
        std::string f;
        str >> f;
        if (f.find("map_Kd") != std::string::npos)
        {
            str >> f;
            auto vec = MyUtil::splitString(f,'/');
            f = vec[vec.size()-1];
            ptr->textures.push_back(Texture::getTexture(std::string("./resources/textures/")+f));
        }
    } while (stream.peek()!=EOF);
}


void OBJParser::getVertexes(std::ifstream stream,
std::vector<glm::vec3>& prephase,
std::unordered_map<int_fast16_t,glm::vec3>& premap)
{
    std::string line;
    //don't know how string works under the hood so I do this just incase im reading uninitialized memory
    std::getline(stream,line);
    while (line[0]!='v')
        std::getline(stream,line);
    int i = 1;
    do {
        std::istringstream str(line);
        glm::vec3 vec;
        std::string f;
        str >> f;
        for (int j=0;j<3;j++)
        {
            str >> f;
            vec[j] = std::stof(f);
        }
            
        prephase.push_back(vec);
        premap.emplace(i++,vec);
        std::getline(stream,line);
    } while (line[1]!='n');
}





void OBJParser::getUVs(std::ifstream stream,
std::vector<glm::vec2>& prephase,
std::unordered_map<int_fast16_t,glm::vec2>& premap)
{
    std::string line;
    std::getline(stream,line);
    while (line[1]!='t'||line[0]!='v')
        std::getline(stream,line);
    int i = 1;
    do {
        std::istringstream str(line);
        glm::vec2 vec;
        std::string f;
        str >> f;
        for (int j=0;j<2;j++)
        {
            str >> f;
            vec[j] = std::stof(f);
        }
            
        prephase.push_back(vec);
        premap.emplace(i++,vec);
        std::getline(stream,line);
    } while (line[0]!='s');
}

Hitbox OBJParser::generateHitbox(std::vector<glm::vec3>& prephase)
{
    float mm[6] = {maxfloat,maxfloat,maxfloat,minfloat,minfloat,minfloat};
    for (const glm::vec3& coords : prephase)
    {
        //check x's
        if (coords[0] < mm[0])
            mm[0] = coords[0];
        if (coords[0] > mm[3])
            mm[3] = coords[0];
        //check y's
        if (coords[1] < mm[1])
            mm[1] = coords[1];
        if (coords[1] > mm[4])
            mm[4] = coords[1];
        //check z's
        if (coords[2] < mm[0])
            mm[2] = coords[2];
        if (coords[2] > mm[5])
            mm[5] = coords[2];
    }
    std::vector<glm::vec3> corners = {{mm[0],mm[1],mm[2]},
    {mm[0],mm[1],mm[5]},
    {mm[0],mm[4],mm[2]},
    {mm[0],mm[4],mm[5]},
    {mm[3],mm[1],mm[2]},
    {mm[3],mm[1],mm[5]},
    {mm[3],mm[4],mm[2]},
    {mm[3],mm[4],mm[5]}};
    return Hitbox(std::move(corners));
}
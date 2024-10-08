#include "InterfaceTextureAtlas.h"
Mesh2D* InterfaceTextureAtlas::buildUI(const std::string& file)
{
    std::vector<GLuint> faces = std::vector<GLuint>();
    std::vector<GLfloat> vertexes = std::vector<GLfloat>();
    GLfloat x = -0.5f;
    GLfloat y = -0.5f;
    std::array<GLfloat, 8> UVs = {0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f};
    std::array<GLfloat, 8> XYZs = {-1.0f,-1.0f,1.0f,-1.0f,-1.0f,1.0f,1.0f,1.0f};
    for (size_t j = 0; j<8; j+=2)
    {
        vertexes.push_back(XYZs[j]);
        vertexes.push_back(XYZs[j+1]);
        vertexes.push_back(UVs[j]);
        vertexes.push_back(UVs[j+1]);
    }
    faces.push_back(0);
    faces.push_back(2);
    faces.push_back(3);
    faces.push_back(1);
    std::shared_ptr<Shader> shader = Shader::getShader(std::string("./resources/shaders/singleTexture"));
    Mesh2D* ptr = new Mesh2D(std::move(VBO(std::move(vertexes))),std::move(EBO(std::move(faces))),std::move(VAO(shader)),shader,GL_QUADS);
    ptr->textures.push_back(Texture::getTexture(file));
    return ptr;
}
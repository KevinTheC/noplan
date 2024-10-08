#include "Interface.h"
Interface::Interface(std::map<Mesh2D*,void(*)(Interface*)> regions, std::map<bool(*)(GLFWwindow*),void(*)()> keybinds)
{
    this->regions = regions;
    this->keybinds = keybinds;
}
bool Interface::attemptKey(GLFWwindow* window, Event event)
{
    for (auto const& value : keybinds)
    {
        if (value.first(window))
        {
            value.second();
            return true;
        }
    }
    return false;
}
bool Interface::attemptClick(GLFWwindow* window, float x, float y, int button)
{
    for (auto const& val : regions)
    {
        if (bounds(window,val.first).isInBounds(x,y))
        {
            
            val.second(this);
            return true;  
        }
    }
    return false;
}
void Interface::draw() const
{
    for (auto const& val : drawables)
        val->draw();
}
Area Interface::bounds(GLFWwindow* window, Mesh2D* mesh)
{
    int width, height;
    glfwGetWindowSize(window,&width,&height);
    glm::mat4 model = mesh->getModel();
    std::vector<GLfloat> values = mesh->getVBO().getValues();
    int stride = mesh->getVAO().getStride();
    float xmin = -300000000;
    float xmax = MAX_FLOAT;
    float ymin = -300000000;
    float ymax = MAX_FLOAT;
    for (size_t i = 0; i < values.size(); i+= stride)
    {
        if (values[i] > xmax)
            values[i] = xmax;
        if (values[i] < xmin)
            values[i] = xmin;
        if (values[i+1] > ymax)
            values[i+1] = ymax;
        if (values[i+1] < ymin)
            values[i+1] = ymin;
    }
    float ifWidth = (xmax - xmin) * width * model[0][0];
    float ifHeight = (ymax - ymin) * height * model[1][1];
    Area area;
    area.left = (-ifWidth*0.5f)+(ifWidth*model[0][3]);
    area.right = area.left + ifWidth;
    area.top = (-ifHeight*0.5f)+(ifHeight*model[1][3]);
    area.bottom = area.top + ifHeight;
    return area;
}

Interface::~Interface()
{
    while (!drawables.empty())
    {
        Mesh2D* m = drawables.back();
        drawables.pop_back();
        delete m;
    }
}
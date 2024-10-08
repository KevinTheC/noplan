#include "Shader.h"
std::string get_file_contents(std::string filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}
GLuint Shader::current = 0;
Shader::Shader(std::string file)
{
	std::string vertexCode = get_file_contents(file + ".vert");
	std::string fragmentCode = get_file_contents(file + ".frag");

	auto str = vertexCode.substr(vertexCode.find('/') + 2, (vertexCode.find('l') - 2) - (vertexCode.find('/') + 2));
	int loc = 0;
	while (loc < str.size())
	{
		attribs.push_back(str.at(loc) - '0');
		loc += 2;
	}

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();
	//reference similar to wxwidgets, creates vertex shader, attaches to its source, compiles its source into machine code
	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	compileError(vertexShader, "VERTEX");

	// same thing for fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	compileError(fragmentShader, "FRAGMENT");

	//create the shader program and attach the shaders to it, links it as well probably some assembly shit idk
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	//delete the objects, already exist in the program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	LOG_DEBUG("Shaders parsed at: "+file);
}


bool Shader::activate()
{
	if (Shader::current == ID)
		return false;
	Shader::current = ID;
	glUseProgram(ID);
	return true;
}
void Shader::erase()
{
	glDeleteProgram(ID);
}
void Shader::compileError(unsigned int shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];
	glGetShaderInfoLog(shader, 1024, NULL, infoLog);
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
			LOG_ALL("Shader compilation error for: "+ Logger::toString(type) + "\n" + infoLog);
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
			LOG_ALL("Shader compilation error for: "+ Logger::toString(type) + "\n" + infoLog);
	}
}
std::vector<int> Shader::getAttribs()
{
	return attribs;
}
std::shared_ptr<Shader> Shader::getShader(std::string file)
{
	if (map.find(file)!=map.end())
        return map.at(file);
    map.emplace(file,std::shared_ptr<Shader>(new Shader(file)));
    return map.at(file);
}
void Shader::clearCache()
{
	map.clear();
}
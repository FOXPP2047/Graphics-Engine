//#include "Shader.hpp"
//
//#include <fstream>
//#include <sstream>
//#include <stdexcept>
//
//#include <glm/gtc/type_ptr.hpp>
//
//std::string load_shader(const std::string &path)
//{
//    std::ifstream file(path);
//    if (!file)
//        throw std::runtime_error("Failed to open the shader source file");
//
//    std::stringstream ss;
//    ss << file.rdbuf();
//
//    file.close();
//
//    return ss.str();
//}
//
//void checkShader(const GLuint shader)
//{
//    int success;
//    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//
//    if (!success)
//    {
//        char info_log[512];
//        glGetShaderInfoLog(shader, 512, nullptr, info_log);
//
//        std::string error = "Failed to compile the shader: " + std::string(info_log);
//        throw std::runtime_error(error);
//    }
//}
//
//GLuint compileShader(const std::string &shader_src, const int shader_type)
//{
//    const char *shader_src_cstr = shader_src.c_str();
//
//    GLuint shader = glCreateShader(shader_type);
//    glShaderSource(shader, 1, &shader_src_cstr, nullptr);
//    glCompileShader(shader);
//
//    checkShader(shader);
//
//    return shader;
//}
//
//Shader::Shader(const std::string &vertex_path, const std::string &fragment_path)
//{
//    const std::string vertex_src = load_shader(vertex_path),
//                      fragment_src = load_shader(fragment_path);
//
//    GLuint vertex = compileShader(vertex_src, GL_VERTEX_SHADER),
//           fragment = compileShader(fragment_src, GL_FRAGMENT_SHADER);
//
//    program_id = glCreateProgram();
//    glAttachShader(program_id, vertex);
//    glAttachShader(program_id, fragment);
//    glLinkProgram(program_id);
//
//    glDeleteShader(vertex);
//    glDeleteShader(fragment);
//}
//
//void Shader::use() const
//{
//    glUseProgram(program_id);
//}
//
//void Shader::setMat4(const std::string &uniform_name, const glm::mat4 &matrix) const
//{
//    GLint uniform_location = getUniformLocation(uniform_name);
//    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
//}
//
//void Shader::setVec3(const std::string &uniform_name, const glm::vec3& value) const
//{
//    glUniform3fv(getUniformLocation(uniform_name), 1, &value[0]);
//}
//
//GLint Shader::getUniformLocation(const std::string &uniform_name) const
//{
//    const char *uniform_name_cstr = uniform_name.c_str();
//    return glGetUniformLocation(program_id, uniform_name_cstr);
//}
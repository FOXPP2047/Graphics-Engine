#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <glad/glad.h>

struct Texture
{
    Texture(const std::string &path);
    void use() const;

    GLuint texture;
};

#endif
/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Sphere.hpp
Purpose: header file for draw Sphere
Language: c++
Platform: win64
Project: yongmin.cho_CS300_1
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.10.04
End Header --------------------------------------------------------*/
#pragma once

const float PI = 3.141592f;

#include <vector>
#include "glm/glm.hpp"

class Sphere
{
public:
    Sphere(float radius = 0.3f, int sectorCount = 36, int stackCount = 36, bool smooth = true);
    ~Sphere() {}

    void draw();
    float radius;
    int sectorCount, stackCount;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;

    std::vector<glm::ivec3> faces;

    unsigned int VAO;
    unsigned int indexBuffer;
private:
};
/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Sphere.cpp
Purpose: To draw Sphere
Language: c++
Platform: win64
Project: yongmin.cho_CS300_1
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.10.04
End Header --------------------------------------------------------*/
#include "include/glad/glad.h"

#include "Sphere.hpp"

Sphere::Sphere(float radius, int sectorCount, int stackCount, bool smooth)
{
    this->radius = radius;
    this->sectorCount = sectorCount;
    this->stackCount = stackCount;
    this->VAO = 0;

    std::vector<glm::vec3>().swap(vertices);
    std::vector<glm::vec3>().swap(normals);
    std::vector<glm::ivec3>().swap(faces);

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glm::vec3 temp;
    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            
            temp[0] = x;
            temp[1] = y;
            temp[2] = z;
            
            vertices.push_back(temp);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            temp[0] = nx;
            temp[1] = ny;
            temp[2] = nz;
            
            normals.push_back(temp);
        }
    }

    int k1, k2;
    glm::ivec3 itemp;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                itemp[0] = k1;
                itemp[1] = k2;
                itemp[2] = k1 + 1;

                faces.push_back(itemp);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                itemp[0] = k1 + 1;
                itemp[1] = k2;
                itemp[2] = k2 + 1;

                faces.push_back(itemp);
            }
        }
    }
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // position
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->faces.size() * sizeof(glm::ivec3), &this->faces[0], GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // position
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sphere::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(faces.size() * 3), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
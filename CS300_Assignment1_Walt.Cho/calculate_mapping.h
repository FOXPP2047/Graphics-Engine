/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: calculate_mapping.h
Purpose: calculate mapping
Language: c++
Platform: win64
Project: yongmin.cho_CS300_2
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.11.15
End Header --------------------------------------------------------*/
#pragma once

#include "glm/glm.hpp"

glm::vec2 calculate_cylindrical_mapping(glm::vec3 entity)
{
    float theta = atan(entity.y / entity.x);
    float z = (entity.z + 1) / 2.0f;

    glm::vec2 uv = glm::vec2(0.0f);

    uv.x = theta / glm::radians(360.f);
    uv.y = z;

    return uv;
}

glm::vec2 calculate_spherical_mapping(glm::vec3 entity)
{
    float theta = atan(entity.y / entity.x);

    float r = sqrt(entity.x * entity.x + entity.y * entity.y + entity.z * entity.z);
    float phi = acos(entity.z / r);

    glm::vec2 uv = glm::vec2(0.0f);

    uv.x = theta / glm::radians(360.f);
    uv.y = (glm::radians(180.f) - phi) / glm::radians(180.f);

    return uv;
}

glm::vec2 calculate_cube_mapping(glm::vec3 entity)
{
    glm::vec3 abs_entity = abs(entity);
    glm::vec2 uv = glm::vec2(0.0f);

    // +- X
    if (abs_entity.x > abs_entity.y && abs_entity.x > abs_entity.z)
    {
        (entity.x < 0.0f) ? (uv.x = entity.z) : (uv.x = -entity.z);
        uv.y = entity.y;
    }
    // +- Y
    else if (abs_entity.y > abs_entity.x && abs_entity.y > abs_entity.z)
    {
        uv.x = entity.x;
        (entity.y < 0.0f) ? (uv.y = entity.z) : (uv.y = -entity.z);
    }
    // +- Z
    else if (abs_entity.z >= abs_entity.x && abs_entity.z >= abs_entity.y)
    {
        (entity.z < 0.0f) ? (uv.x = -entity.x) : (uv.x = entity.x);
        uv.y = entity.y;
    }

    //Convert range from [-1, 1] to [0, 1]
    return (uv + glm::vec2(1.0f)) * 0.5f;
}

/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: render_data.h
Purpose: uniform blocks for render_data
Language: c++
Platform: win64
Project: yongmin.cho_CS300_2
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.11.15
End Header --------------------------------------------------------*/
#pragma once

#include "glm/glm.hpp"
const int MAX_LIGHT = 16;

enum Light_type { point = 0, spot = 1, directional = 2 };

struct render_data
{
    Light_type type;
    glm::vec3 ambient, diffuse, specular;
    glm::vec3 light_pos;

    glm::vec3 light_color;
    //Direction for spot and directional
    glm::vec3 direction;

    //Inner and outer angle for spot light
    float inner;
    float outer;

    float falloff;
};

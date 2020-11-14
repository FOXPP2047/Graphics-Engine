/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.cpp
Purpose: To draw Model and read file and load model
Language: c++
Platform: win64
Project: yongmin.cho_CS300_1
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.10.04
End Header --------------------------------------------------------*/
#include "Model.hpp"
#include "include/glad/glad.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "Camera.hpp"
#include "Shader.hpp"
#include "calculate_mapping.h"
#include <limits>
static bool is_trianglefan = false;

void Model::load_model(const std::string &obj_path)
{
    std::ifstream file(obj_path);
    //first_time = true;
    if (!file)
        throw std::runtime_error("Failed to open the model file");

    for (int i = 0; i < 3; ++i)
    {
        min[i] = std::numeric_limits<float>::max();
        max[i] = -std::numeric_limits<float>::max();
    }
    
    std::string line;
    std::string temp;
    vertices.clear();
    vertex_normal.clear();
    faces.clear();
    for_draw_vn.clear();
    while(!file.eof())
    {
        std::getline(file, line);
        //std::stringstream ss(line);
        //std::stringstream ss_remian(line);
        //std::string trash;
        //ss >> trash; // gets rid of v/vn/vt/f

        if (line.compare(0, 1, "#") == 0)
            continue;
        else if (line.compare(0, 1, "g") == 0)
            continue;
        else if (line.compare(0, 1, "s") == 0)
            continue;
        else if (line.compare(0, 2, "v ") == 0 || (line.compare(0, 2, "v\t")) == 0)
        {
            
            glm::vec3 vec;
            line.erase(0, 2);
            std::stringstream ss_remian(line);
            if (first_time)
            {
                for (int i = 0; i < 3; ++i)
                {
                    ss_remian >> vec[i];
                    min[i] = vec[i];
                    max[i] = vec[i];
                }
                first_time = false;
            }
            else
            {
                for (int i = 0; i < 3; ++i)
                {
                    ss_remian >> vec[i];
                    if (min[i] > vec[i])
                        min[i] = vec[i];
                    else if (vec[i] > max[i])
                        max[i] = vec[i];
                }
            }

            vertices.push_back(vec);
            vertex_normal.push_back(glm::vec3(0.f));
        }
        else if (line.compare(0, 3, "vn ") == 0)
        {
            glm::vec3 vec;
            line.erase(0, 2);
            std::stringstream ss(line);
            for (int i = 0; i < 3; i++)
                ss >> vec[i];

            normals.push_back(vec);
        }
        else if (line.compare(0, 3, "vt ") == 0)
        {
            glm::vec2 vec;
            std::stringstream ss(line);
            for (int i = 0; i < 2; i++)
                ss >> vec[i];

            uvs.push_back(vec);
        }

        else if (line.find("//") != std::string::npos)
        {
            int total = 0;
            for (int i = 0; i < line.size(); ++i)
            {
                if (line[i] == ' ')
                    ++total;
            }
            if (total == 3)
            {
                glm::ivec3 ivec;
                int vn[3];
                int vt[3];
                int len = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &vn[0], &vt[0], &vn[1], &vt[1], &vn[2], &vt[2]);
                ivec[0] = vn[0] - 1;
                ivec[1] = vn[2] - 1;
                ivec[2] = vn[1] - 1;
                
                faces.push_back(ivec);

            }

            else if (total == 4)
            {
                glm::ivec3 ivec;
                int vn[4];
                int vt[4];
                int len = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d", &vn[0], &vt[0], &vn[1], &vt[1], &vn[2], &vt[2], &vn[3], &vt[3]);

                for (int i = 1; i < total - 2; ++i)
                {
                    ivec[0] = vn[0] - 1;
                    ivec[1] = vn[total - i] - 1;
                    ivec[2] = vn[total - i - 1] - 1;
                    faces.push_back(ivec);
                }
                ivec[0] = vn[0] - 1;
                ivec[1] = vn[2] - 1;
                ivec[2] = vn[1] - 1;
                faces.push_back(ivec);
                
            }
            else if (total == 5)
            {
                glm::ivec3 ivec;
                int vn[5];
                int vt[5];
                int len = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d %d//%d", &vn[0], &vt[0], &vn[1], &vt[1], &vn[2], &vt[2], &vn[3], &vt[3], &vn[4], &vt[4]);
                

                for (int i = 1; i < total - 2; ++i)
                {
                    ivec[0] = vn[0] - 1;
                    ivec[1] = vn[total - i] - 1;
                    ivec[2] = vn[total - i - 1] - 1;
                    faces.push_back(ivec);
                }
                ivec[0] = vn[0] - 1;
                ivec[1] = vn[2] - 1;
                ivec[2] = vn[1] - 1;
                faces.push_back(ivec);

            }
            else if (total == 6)
            {
                glm::ivec3 ivec;
                int vn[6];
                int vt[6];
                int len = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d",
                    &vn[0], &vt[0], &vn[1], &vt[1], &vn[2], &vt[2], &vn[3], &vt[3], &vn[4], &vt[4], &vn[5], &vt[5]);

                for (int i = 1; i < total - 2; ++i)
                {
                    ivec[0] = vn[0] - 1;
                    ivec[1] = vn[total - i] - 1;
                    ivec[2] = vn[total - i - 1] - 1;
                    faces.push_back(ivec);
                }
                ivec[0] = vn[0] - 1;
                ivec[1] = vn[2] - 1;
                ivec[2] = vn[1] - 1;
                faces.push_back(ivec);
            }
            else if (total == 7)
            {
                glm::ivec3 ivec;
                int vn[7];
                int vt[7];
                int len = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d",
                    &vn[0], &vt[0], &vn[1], &vt[1], &vn[2], &vt[2], &vn[3], &vt[3], &vn[4], &vt[4], &vn[5], &vt[5], &vn[6], &vt[6]);

                for (int i = 1; i < total - 2; ++i)
                {
                    ivec[0] = vn[0] - 1;
                    ivec[1] = vn[total - i] - 1;
                    ivec[2] = vn[total - i - 1] - 1;
                    faces.push_back(ivec);
                }
                faces.push_back(ivec);

            }
            else if (total == 8)
            {
                glm::ivec3 ivec;
                int vn[8];
                int vt[8];
                int len = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d",
                    &vn[0], &vt[0], &vn[1], &vt[1], &vn[2], &vt[2], &vn[3], &vt[3], &vn[4], &vt[4], &vn[5], &vt[5], &vn[6], &vt[6], &vn[7], &vt[7]);

                for (int i = 1; i < total - 2; ++i)
                {
                    ivec[0] = vn[0] - 1;
                    ivec[1] = vn[total - i] - 1;
                    ivec[2] = vn[total - i - 1] - 1;
                    faces.push_back(ivec);
                }
                ivec[0] = vn[0] - 1;
                ivec[1] = vn[2] - 1;
                ivec[2] = vn[1] - 1;
                faces.push_back(ivec);

            }
            else if (total == 9)
            {
                glm::ivec3 ivec;
                int vn[9];
                int vt[9];
                int len = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d",
                    &vn[0], &vt[0], &vn[1], &vt[1], &vn[2], &vt[2], &vn[3], &vt[3], &vn[4], &vt[4], &vn[5], &vt[5], 
                    &vn[6], &vt[6], &vn[7], &vt[7], &vn[8], &vt[8]);

                for (int i = 1; i < total - 2; ++i)
                {
                    ivec[0] = vn[0] - 1;
                    ivec[1] = vn[total - i] - 1;
                    ivec[2] = vn[total - i - 1] - 1;
                    faces.push_back(ivec);
                }
                ivec[0] = vn[0] - 1;
                ivec[1] = vn[2] - 1;
                ivec[2] = vn[1] - 1;
                faces.push_back(ivec);
            }
            else if (total == 10)
            {
                glm::ivec3 ivec;
                int vn[10];
                int vt[10];
                int len = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d",
                    &vn[0], &vt[0], &vn[1], &vt[1], &vn[2], &vt[2], &vn[3], &vt[3], &vn[4], &vt[4], &vn[5], &vt[5], 
                    &vn[6], &vt[6], &vn[7], &vt[7], &vn[8], &vt[8], &vn[9], &vt[9]);

                for (int i = 1; i < total - 2; ++i)
                {
                    ivec[0] = vn[0] - 1;
                    ivec[1] = vn[total - i] - 1;
                    ivec[2] = vn[total - i - 1] - 1;
                    faces.push_back(ivec);
                }
                ivec[0] = vn[0] - 1;
                ivec[1] = vn[2] - 1;
                ivec[2] = vn[1] - 1;
                faces.push_back(ivec);

            }
        }
        else if (line.compare(0, 2, "f ") == 0)
        {
            if (line.find("/") != std::string::npos)
            {
                glm::ivec3 ivec;
                int v[3];
                int vt[3];
                int vn[3];
                int len = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2]);
                ivec[0] = v[0] - 1;
                ivec[1] = v[2] - 1;
                ivec[2] = v[1] - 1;
                faces.push_back(ivec);
            }

            else
            {
                glm::ivec3 ivec;
                line.erase(0, 2);
                std::stringstream ss(line);
                std::stringstream ss2(line);
                int total = 0;
                temp.clear();

                for (int i = 0; !ss2.eof(); ++i)
                {
                    ss2 >> line;

                    if (line[0] == '\\')
                        break;
                    else if (line[0] == '/')
                        continue;
                    else if (line[0] != ' ')
                    {
                        ++total;
                    }

                }
                if (total < 4)
                {
                    for (int i = 0; !ss.eof(); ++i)
                    {
                        if (ss.peek() != '\\')
                        {
                            ss >> ivec[i];
                            ivec[i]--;
                        }
                        else if (ss.peek() == '\\')
                            break;
                    }
                    faces.push_back(ivec);
                }

                else
                {
                    ss >> ivec[0];
                    ss >> ivec[1];
                    ss >> ivec[2];
                    ivec[0]--;
                    ivec[1]--;
                    ivec[2]--;
                    faces.push_back(ivec);
                    const int first = ivec[0];
                    int start = ivec[2];
                    bool first_start = true;
                    for (int i = 0; i < total - 3; ++i)
                    {
                        ivec[0] = first;

                        if (first_start)
                        {
                            ivec[1] = ivec[2];
                            first_start = false;
                        }
                        else ivec[1] = start;
                        ss >> ivec[2];
                        ivec[2]--;
                        start = ivec[2];

                        faces.push_back(ivec);
                    }
                }
            }

        }
    }
}

void Model::set_up_VAO()
{
    //vertices
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    if (this->move)
    {
        glm::vec3 middle;
        for (int i = 0; i < 3; ++i)
            middle[i] = (max[i] - min[i]) * 0.5f;
        this->move_middle = middle;
        glm::vec3 true_middle;
        for (int i = 0; i < 3; ++i)
            true_middle[i] = max[i] - middle[i];
        true_middle.x ? (0.5f / true_middle.x) : 1.f;
        true_middle.y ? (0.5f / true_middle.y) : 1.f;
        true_middle.z ? (0.5f / true_middle.z) : 1.f;

        float minest_offest = glm::min(glm::min(true_middle.x, true_middle.y), true_middle.z);

        for (std::vector<glm::vec3>::iterator it = vertices.begin(); it != vertices.end(); ++it)
        {
            //move
            it->x -= middle.x;
            it->y -= middle.y;
            it->z -= middle.z;
            //scaling
            it->x *= 1 / ((middle.x + middle.y + middle.z) / 3);
            it->y *= 1 / ((middle.x + middle.y + middle.z) / 3);
            it->z *= 1 / ((middle.x + middle.y + middle.z) / 3);
        }
    }

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // position
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->faces.size() * sizeof(glm::ivec3), &this->faces[0], GL_STATIC_DRAW);

    //vertex normal
    unsigned int v_VBO;
    glGenBuffers(1, &v_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, v_VBO);

    if (this->move)
    {
        FaceNormal temp;
        for (std::vector<glm::ivec3>::iterator it = faces.begin(); it != faces.end(); it++)
        {
            // center
            temp.pos = (vertices[it->x] + vertices[it->y] + vertices[it->z]) / 3.f;
            temp.normal_vec = glm::normalize(glm::cross(vertices[it->y] - vertices[it->x], vertices[it->z] - vertices[it->x]));
            glm::vec3 normal = (temp.normal_vec * 0.4f + temp.pos);
            face_normal.push_back(temp.pos);
            face_normal.push_back(normal);

            temp_face_normal.push_back(temp.normal_vec);
        }

        int j = 0;
        for (std::vector<glm::ivec3>::iterator it = faces.begin(); it != faces.end(); ++it, ++j)
        {
            vertex_normal[it->x] += temp_face_normal[j];
            vertex_normal[it->y] += temp_face_normal[j];
            vertex_normal[it->z] += temp_face_normal[j];
        }

        for (int i = 0; i < vertex_normal.size(); ++i)
            vertex_normal[i] = glm::normalize(vertex_normal[i]);

        glBufferData(GL_ARRAY_BUFFER, vertex_normal.size() * sizeof(glm::vec3), vertex_normal.data(), GL_STATIC_DRAW);
    }
    else glBufferData(GL_ARRAY_BUFFER, faces.size() * sizeof(glm::vec3), faces.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    //face normal
    glGenVertexArrays(1, &face_normal_VAO);
    glBindVertexArray(face_normal_VAO);

    unsigned int f_VBO;
    glGenBuffers(1, &f_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, f_VBO);

    glBufferData(GL_ARRAY_BUFFER, face_normal.size() * sizeof(glm::vec3), face_normal.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    //Cylindrical
    //Normal
    unsigned int normal_uv_VBO;
    glGenBuffers(1, &normal_uv_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normal_uv_VBO);
    std::vector<glm::vec2> cy_norm;
    for (int i = 0; i < face_normal.size(); ++i)
        cy_norm.emplace_back(calculate_cylindrical_mapping(face_normal[i]));
    glBufferData(GL_ARRAY_BUFFER, cy_norm.size() * sizeof(glm::vec2), cy_norm.data(), GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, normal_uv_VBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);
    
    //Pos
    unsigned int pos_uv_VBO;
    glGenBuffers(1, &pos_uv_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, pos_uv_VBO);
    std::vector<glm::vec2> cy_pos;
    for (int i = 0; i < face_normal.size(); ++i) 
        cy_pos.emplace_back(calculate_cylindrical_mapping(face_normal[i]));
    glBufferData(GL_ARRAY_BUFFER, cy_pos.size() * sizeof(glm::vec2), cy_pos.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, pos_uv_VBO);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(3);

    //vertex normal draw
    glGenVertexArrays(1, &vertex_normal_VAO);
    glBindVertexArray(vertex_normal_VAO);
    unsigned int v_VBO_draw;
    glGenBuffers(1, &v_VBO_draw);
    glBindBuffer(GL_ARRAY_BUFFER, v_VBO_draw);

    int s = 0;
    for (std::vector<glm::vec3>::iterator it = vertices.begin(); it != vertices.end(); ++it, ++s)
    {
        for_draw_vn.push_back(vertices[s]);
        for_draw_vn.push_back(vertices[s] + vertex_normal[s] * 0.4f);
    }
    
    glBufferData(GL_ARRAY_BUFFER, for_draw_vn.size() * sizeof(glm::vec3), for_draw_vn.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

}

void Model::draw() const
{
    //Draw Model
    glBindVertexArray(VAO);
    if(is_trianglefan)
        glDrawElements(GL_TRIANGLE_FAN, static_cast<GLsizei>(faces.size() * 3), GL_UNSIGNED_INT, 0);
    else glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(faces.size() * 3), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //Draw Face Normal
    if (is_faceNormal)
    {
        glBindVertexArray(face_normal_VAO);

        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(face_normal.size()));
        glBindVertexArray(0);
    }


    //Draw Vertex Normal
    if (is_vertexNormal)
    {
        glBindVertexArray(vertex_normal_VAO);

        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(for_draw_vn.size()));
        glBindVertexArray(0);
    }
}
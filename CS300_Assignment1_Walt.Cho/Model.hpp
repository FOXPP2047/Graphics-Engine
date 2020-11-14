/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.hpp
Purpose: header file of To draw Model and read file and load model
Language: c++
Platform: win64
Project: yongmin.cho_CS300_1
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.10.04
End Header --------------------------------------------------------*/
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

extern bool is_faceNormal;
extern bool is_vertexNormal;

//static GLuint skybox_texture[6];

struct VertexBuffer
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct FaceNormal
{
    glm::vec3 pos;
    glm::vec3 normal_vec;
};

struct Model
{
    Model() {}
    void draw() const;
    bool first_time = true;
    glm::vec3 min;
    glm::vec3 max;
    unsigned int VAO = 0;
    unsigned int face_normal_VAO = 0;
    unsigned int vertex_normal_VAO = 0;
    unsigned int indexBuffer = 0;

    unsigned int normal_uv = 0;
    unsigned int pos_uv = 0;

    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec3> position_save;
    std::vector<glm::vec2> uvs;
    std::vector<glm::ivec3> faces;
    std::vector<glm::vec3> face_normal;
    std::vector<glm::vec3> vertex_normal;

    std::vector<glm::vec3> temp_face_normal;
    std::vector<glm::vec3> for_draw_vn;

    glm::vec3 move_middle;

	static bool set_up_fbo();
	static void use_fbo(unsigned int fbo, int view_width, int view_height);
    void load_model(const std::string &obj_path);
    void set_up_VAO();
    void draw_skybox();
    bool move = true;
};
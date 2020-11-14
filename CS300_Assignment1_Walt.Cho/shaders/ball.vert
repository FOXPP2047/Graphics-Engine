/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ball.vert
Purpose: vertex shader of ball(for oribiting 8 balls)
Language: GLSL
Platform: win64
Project: yongmin.cho_CS300_1
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.10.04
End Header --------------------------------------------------------*/
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 0) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec4 out_pos;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 rotate;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; 

out vec3 colour_object;
out vec3 colour_light;
out vec3 pos_light;

void main()
{
	Normal = aNormal;
	colour_object = objectColor;
    colour_light = lightColor;
    pos_light = lightPos;
	FragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * transform * rotate * vec4(FragPos, 1.0);
	out_pos = gl_Position;
}
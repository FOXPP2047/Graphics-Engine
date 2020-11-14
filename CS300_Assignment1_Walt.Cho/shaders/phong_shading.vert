/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phong_shading.vert
Purpose: vertex shader for phong shading
Language: GLSL
Platform: win64
Project: yongmin.cho_CS300_2
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.11.15
End Header --------------------------------------------------------*/
#version 330 core
//phong shading
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 Pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightColor; 
uniform int num_sphere;

out vec3 colour_object;
out vec3 colour_light;
flat out int count;

void main()
{
	Pos = aPos;
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = /*mat3(transpose(inverse(model))) * */aNormal;  
    colour_light = lightColor;
	count = num_sphere;
	
    gl_Position = projection * view * model * vec4(FragPos, 1.0);
}
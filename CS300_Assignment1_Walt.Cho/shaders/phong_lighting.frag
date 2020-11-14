/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phong_lighting.frag
Purpose: fragment shader for phong lighting
Language: GLSL
Platform: win64
Project: yongmin.cho_CS300_2
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.11.15
End Header --------------------------------------------------------*/
#version 330 core

out vec4 FragColor;

in vec3 phong_lighting_color;
in vec3 not_normal_viewDir;
flat in int count;

uniform float z_far;
uniform float z_near;
uniform vec3 I_fog;

void main()
{
	float length_v = length(not_normal_viewDir);
	float s = (z_far - length_v) / (z_far - z_near);
	
	vec3 color = s * phong_lighting_color + (1 - s) * I_fog;
    FragColor = vec4(color, 1.0);
} 
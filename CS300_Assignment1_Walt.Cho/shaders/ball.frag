/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ball.frag
Purpose: fragment shader of ball(for oribiting 8 balls)
Language: GLSL
Platform: win64
Project: yongmin.cho_CS300_1
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.10.04
End Header --------------------------------------------------------*/
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 colour_light;
in vec3 pos_light;
in vec3 colour_object;
in vec4 out_pos;

void main()
{
	// ambient
    //float ambientStrength = 0.1;
    //vec3 ambient = ambientStrength * colour_light;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pos_light - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = colour_object;//diff * colour_object;
        
    vec3 result = (/*ambient + */diffuse) * colour_object;
    FragColor = vec4(result, 1.0);
}
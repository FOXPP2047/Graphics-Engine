/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phong_lighting.vert
Purpose: vertex shader for phong lighting
Language: GLSL
Platform: win64
Project: yongmin.cho_CS300_2
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.11.15
End Header --------------------------------------------------------*/
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aCy_Norm;
layout (location = 3) in vec2 aCy_Pos;

#define MAX_LIGHT 16

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct render_data
{
    int type;
    vec3 ambient, diffuse, specular;
    vec3 light_pos;

	vec3 light_color;
    //Direction for spot and directional
    vec3 direction;

    //Inner and outer angle for spot light
    float inner;
    float outer;

    float falloff;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int num_sphere;

uniform render_data render_datas[MAX_LIGHT];
uniform Material material;

uniform vec3 viewPos;
uniform float constant;
uniform float linear;
uniform float quadratic;

out vec3 phong_lighting_color;
out vec3 not_normal_viewDir;

uniform int cpu_shading;
uniform int select_mapping;
uniform int normal_position;
uniform int normal_pos;

vec2 calculate_cylindrical_mapping(vec3 entity)
{
	float theta = atan(entity.y / entity.x);
	float z = (entity.z + 1) / 2.0f;
	
	vec2 uv = vec2(0.0);
	
	uv.x = theta / radians(360.f);
	uv.y = z;
	
	return (uv + vec2(1.0f)) * 0.5f;
}

vec2 calculate_spherical_mapping(vec3 entity)
{
	float theta = atan(entity.y / entity.x);
	
	float r = sqrt(entity.x * entity.x + entity.y * entity.y + entity.z * entity.z);
	float phi = acos(entity.z / r);
	
	vec2 uv = vec2(0.0);
	
	uv.x = theta / radians(360.f);
	uv.y = (radians(180.f) - phi) / radians(180.f);
	
	return (uv + vec2(1.0f)) * 0.5;
}

vec2 calculate_cube_mapping(vec3 entity)
{
	vec3 abs_entity = abs(entity);
	vec2 uv = vec2(0.0);
	
	// +- X
	if(abs_entity.x > abs_entity.y && abs_entity.x > abs_entity.z)
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
	else if (abs_entity.z > abs_entity.x && abs_entity.z > abs_entity.y)
	{
		(entity.z < 0.0f) ? (uv.x = -entity.x) : (uv.x = entity.x);
		uv.y = entity.y;
	}
	
	//Convert range from [-1, 1] to [0, 1]
	return (uv + vec2(1.0f)) * 0.5f;
}

vec3 reflect_mine(vec3 normal, vec3 viewDir)
{
	return (2 * dot(normal, viewDir) * normal - viewDir);
}

vec3 phong_lighting()
{
	//float ambientStrength = 0.1;
	vec3 norm = normalize(aNormal);
	
	vec3 result = vec3(0.f);
	vec3 FragPos = vec3(model * vec4(aPos, 1.0));
	
	vec2 uv = vec2(0.f);
	
	if(cpu_shading == 0)
	{
		if(select_mapping == 0)
		{
			if(normal_pos == 0)
				uv = calculate_cylindrical_mapping(aNormal);
			else if(normal_pos == 1)
				uv = calculate_cylindrical_mapping(aPos);
		}
			
		else if(select_mapping == 1)
		{
			if(normal_pos == 0)
				uv = calculate_spherical_mapping(aNormal);
			else if(normal_pos == 1)
				uv = calculate_spherical_mapping(aPos);
		}
		
		//cube mapping
		else if(select_mapping == 2)
		{
			if(normal_pos == 0)
				uv = calculate_cube_mapping(aNormal);
			else if(normal_pos == 1)
				uv = calculate_cube_mapping(aPos);
		}
		
	}
	else if(cpu_shading == 1)
	{
		if(select_mapping == 0)
		{
			if(normal_pos == 0)
				uv = calculate_cylindrical_mapping(aNormal);
			else if(normal_pos == 1)
				uv = calculate_cylindrical_mapping(aPos);
		}
			
		else if(select_mapping == 1)
		{
			if(normal_pos == 0)
				uv = calculate_spherical_mapping(aNormal);
			else if(normal_pos == 1)
				uv = calculate_spherical_mapping(aPos);
		}
		
		//cube mapping
		else if(select_mapping == 2)
		{
			if(normal_pos == 0)
				uv = calculate_cube_mapping(aNormal);
			else if(normal_pos == 1)
				uv = calculate_cube_mapping(aPos);
		}
	}
	
	for(int i = 0; i < num_sphere; ++i)
	{
		vec3 ambient = render_datas[i].ambient;
		vec3 lightDir = normalize(render_datas[i].light_pos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = render_datas[i].diffuse * diff * vec3(texture(material.diffuse, uv));
		float distance = length(render_datas[i].light_pos - FragPos);
		float att = 1.0 / (constant + linear * distance + quadratic * (distance * distance));	
		
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 not_normal_viewDir = viewPos - FragPos;
		if(render_datas[i].type == 0)
		{
			vec3 reflectDir = reflect_mine(norm, viewDir);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			vec3 specular = render_datas[i].specular * spec * vec3(texture(material.specular, uv));
			result += ((ambient * att + diffuse * att + specular * att) * render_datas[i].light_color);
		}
		else if(render_datas[i].type == 1)
		{						
			//specular
			vec3 reflectDir = reflect_mine(norm, viewDir);
			//vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			vec3 specular = render_datas[i].specular * spec * vec3(texture(material.specular, uv));
			
			//soft edge
			float theta = dot(lightDir, normalize(-render_datas[i].direction));
			float epsilon = render_datas[i].inner - render_datas[i].outer;
			float intensity = pow(clamp((theta - render_datas[i].outer) / epsilon, 0.0, 1.0), render_datas[i].falloff);

			//ambient *= intensity; 
			diffuse *= intensity;
			specular *= intensity;

			ambient *= att; 
			diffuse *= att;
			specular *= att;   
        
			result += ((ambient + diffuse + specular) * render_datas[i].light_color);
		}
		else if(render_datas[i].type == 2)
		{
			vec3 reflectDir = reflect_mine(norm, viewDir);
			//vec3 reflectDir = reflect(-lightDir, norm);  
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			vec3 specular = render_datas[i].specular * spec * vec3(texture(material.specular, uv));  
        
			result += ((ambient + diffuse + specular) * render_datas[i].light_color);
		}
		
	}
	return result;
}
void main()
{
	phong_lighting_color = phong_lighting();
	gl_Position = projection * view * model * vec4(vec3(model * vec4(aPos, 1.0)), 1.0);
}
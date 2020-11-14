/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: blinn.frag
Purpose: fragment shader for blinn shading
Language: GLSL
Platform: win64
Project: yongmin.cho_CS300_2
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.11.15
End Header --------------------------------------------------------*/
#version 330 core

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

out vec4 FragColor;

uniform float constant;
uniform float linear;
uniform float quadratic;
uniform vec3 viewPos;

uniform float z_far;
uniform float z_near;
uniform vec3 I_fog;

in vec3 Normal;  
in vec3 FragPos;
in vec3 Pos;

flat in int count;
uniform render_data render_datas[MAX_LIGHT];
uniform Material material;

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

vec3 blinn_shading()
{
	vec3 norm = normalize(Normal);
	
	vec3 result = vec3(0.0f);
	
	vec2 uv = vec2(0.0f);
	
	if(cpu_shading == 0)
	{
		if(select_mapping == 0)
		{
			if(normal_pos == 0)
				uv = calculate_cylindrical_mapping(Normal);
			else if(normal_pos == 1)
				uv = calculate_cylindrical_mapping(Pos);
		}
			
		else if(select_mapping == 1)
		{
			if(normal_pos == 0)
				uv = calculate_spherical_mapping(Normal);
			else if(normal_pos == 1)
				uv = calculate_spherical_mapping(Pos);
		}
		
		//cube mapping
		else if(select_mapping == 2)
		{
			if(normal_pos == 0)
				uv = calculate_cube_mapping(Normal);
			else if(normal_pos == 1)
				uv = calculate_cube_mapping(Pos);
		}
	}
	else if(cpu_shading == 1)
	{
		if(select_mapping == 0)
		{
			if(normal_pos == 0)
				uv = calculate_cylindrical_mapping(Normal);
			else if(normal_pos == 1)
				uv = calculate_cylindrical_mapping(Pos);
		}
			
		else if(select_mapping == 1)
		{
			if(normal_pos == 0)
				uv = calculate_spherical_mapping(Normal);
			else if(normal_pos == 1)
				uv = calculate_spherical_mapping(Pos);
		}
		
		//cube mapping
		else if(select_mapping == 2)
		{
			if(normal_pos == 0)
				uv = calculate_cube_mapping(Normal);
			else if(normal_pos == 1)
				uv = calculate_cube_mapping(Pos);
		}
	}
	
	for(int i = 0; i < count; ++i)
	{
		float distance = length(render_datas[i].light_pos - FragPos);
		float att = 1.0 / (constant + linear * distance + quadratic * (distance * distance));	
		vec3 ambient = render_datas[i].ambient;
		vec3 lightDir = normalize(render_datas[i].light_pos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = (render_datas[i].diffuse * diff * vec3(texture(material.diffuse, uv)));
		if(render_datas[i].type == 0)
		{		
			vec3 viewDir = normalize(viewPos - FragPos);
			float spec = 0.0;
			
			vec3 halfwayDir = normalize(lightDir + viewDir);
			spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
			
			vec3 specular = (spec * render_datas[i].specular * vec3(texture(material.specular, uv)));
			
			ambient *= att; 
			diffuse *= att;
			specular *= att;
			
			result += ((ambient + diffuse + specular) * render_datas[i].light_color);
		}
		else if(render_datas[i].type == 1)
		{
			//specular
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 halfwayDir = normalize(lightDir + viewDir);
			float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
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
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 halfwayDir = normalize(lightDir + viewDir);
			float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
			vec3 specular = render_datas[i].specular * spec * vec3(texture(material.specular, uv));
        
			result += ((ambient + diffuse + specular) * render_datas[i].light_color);
		}
	}
	return result;	
}

void main()
{
	vec3 not_normal_viewDir = viewPos - FragPos;
	float length_v = length(not_normal_viewDir);
	float s = (z_far - length_v) / (z_far - z_near);
	
	vec3 color = s * blinn_shading() + (1 - s) * I_fog;

    FragColor = vec4(color, 1.0);
}


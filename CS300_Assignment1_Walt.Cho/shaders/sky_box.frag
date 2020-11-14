#version 330 core

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;

uniform sampler2D sky_box[6];
uniform vec3 view_pos;

//uniform bool is_sky_box;

vec3 calculate_uv(vec3 pos, vec3 viewPos)
{
	vec3 uv = vec3(0.5) + 0.5 * (pos - viewPos) * 0.1;
	vec3 determine = abs(pos - viewPos);
	float calculate_texture = 0.f;
	if(determine.x > determine.y && determine.x > determine.z)
	{
		vec2 left = vec2(1 - uv.z, 1 - uv.y);
		vec2 right = vec2(uv.z, 1- uv.y);
		
		calculate_texture = viewPos.x - pos.x;
		if(calculate_texture > 0)
			return vec3(texture(sky_box[3], left));
		else return vec3(texture(sky_box[2], right));
	}
	
	else if(determine.y > determine.z)
	{
		vec2 down = vec2(1 - uv.z, uv.x);
		vec2 up = vec2(1 - uv.z, 1 - uv.x);
		
		calculate_texture = viewPos.y - pos.y;
		if(calculate_texture > 0)
			return vec3(texture(sky_box[5], down));
		else return vec3(texture(sky_box[4], up));
	}
	
	else
	{
		vec2 front = vec2(uv.x, 1 - uv.y);
		vec2 back = vec2(1 - uv.x, 1 - uv.y);
		
		calculate_texture = viewPos.z - pos.z;
		if(calculate_texture > 0)
			return vec3(texture(sky_box[0], front));
		else return vec3(texture(sky_box[1], back));
	}
}

void main()
{
	vec3 out_texture = vec3(0.0);
	out_texture = calculate_uv(FragPos, view_pos);
	FragColor = vec4(out_texture, 1.f);
}
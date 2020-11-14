#version 330 core

const float EtaR = 0.985;
const float EtaG = 1.015;
const float EtaB = 1.045;

const int MIX = 0;
const int REFLECT = 1;
const int REFRACT = 2;

const float FresnelPower = 5.0;

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

const int MAX_LIGHT = 16;

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

uniform int num_sphere;
uniform render_data render_datas[MAX_LIGHT];

uniform vec3 lightPos;
uniform sampler2D env_box[6];
uniform vec3 viewPos;
uniform float material;
uniform vec3 color;

uniform bool is_object;
uniform bool is_add_phong;
uniform bool waveLength;
uniform int toggle;

vec3 reflect_mine(vec3 input_vec);
vec3 GetDirLight(vec3 viewDir, vec3 lightDir, vec3 tex);

void main()
{      
	if (is_object) 
	{
		vec3 out_texture = vec3(0,0,0);
		
		vec3 normal_ = normalize(Normal);
		vec3 View = normalize(viewPos - FragPos);
		float dot_NL = dot(normal_, View);
		
		float K = 1.0 / material;
		float K_R = EtaR / material;
		float K_G = EtaG / material;
		float K_B = EtaB / material;
		float F = ((1-K_G) * (1-K_G)) / ((1+K_G) * (1+K_G));
		float ratio = F + (1.0 - F) * pow((1.0 - dot(View, normal_)), FresnelPower);
		
		float I_Refract = 0;
		float lightIntensity = 0.3;
		
		if(toggle == 0)
		{
			vec3 Reflect = 2 * dot_NL * normal_ - View;
			out_texture = reflect_mine(Reflect);
		}
		
		else if(toggle == 1)
		{
			vec3 refract = vec3(0.0);
			if (waveLength) 
			{
			
				vec3 Refract_R = (K_R*dot_NL - sqrt(1 - K_R*K_R*(1-dot_NL*dot_NL)))*Normal-K_R*View;
				vec3 Refract_G = (K_G*dot_NL - sqrt(1 - K_G*K_G*(1-dot_NL*dot_NL)))*Normal-K_G*View;
				vec3 Refract_B = (K_B*dot_NL - sqrt(1 - K_B*K_B*(1-dot_NL*dot_NL)))*Normal-K_B*View;
			
				refract.r = reflect_mine(Refract_R).r;
				refract.g = reflect_mine(Refract_G).g;
				refract.b = reflect_mine(Refract_B).b;
			}	

			else 
			{

				vec3 Refract = (K*dot_NL - sqrt(1 - K * K * (1 - dot_NL * dot_NL))) * Normal - K * View;
				refract = reflect_mine(Refract);
			}	
			
			I_Refract = K * pow(max(dot(View, refract), 0), 1.f);
			
			out_texture = refract;
		}
		
		else 
		{
			
			vec3 Reflect = 2 * dot_NL * Normal - View;
			
			vec3 refract = vec3(0.0);
			
			if (waveLength) 
			{
			
				vec3 Refract_R = (K_R*dot_NL - sqrt(1 - K_R*K_R*(1-dot_NL*dot_NL)))*Normal-K_R*View;
				vec3 Refract_G = (K_G*dot_NL - sqrt(1 - K_G*K_G*(1-dot_NL*dot_NL)))*Normal-K_G*View;
				vec3 Refract_B = (K_B*dot_NL - sqrt(1 - K_B*K_B*(1-dot_NL*dot_NL)))*Normal-K_B*View;
			
				refract.r = reflect_mine(Refract_R).r;
				refract.g = reflect_mine(Refract_G).g;
				refract.b = reflect_mine(Refract_B).b;
			}	

			else 
			{
				vec3 Refract = (K*dot_NL - sqrt(1 - K*K*(1-dot_NL*dot_NL)))*Normal-K*View;
				refract = reflect_mine(Refract);
			}	
			
			I_Refract = K * pow(max(dot(View, refract), 0), 1.f);
			
			out_texture = mix(refract, 
						reflect_mine(Reflect), 
						ratio);
			out_texture = mix(out_texture, color, ratio);
		}
		
		if (is_add_phong) 
		{
			vec3 light_effect;
			vec3 total;
			for(int i = 0; i < num_sphere; ++i)
			{
				light_effect = (GetDirLight(View, 
								normalize((render_datas[i].light_pos - FragPos)), 
								out_texture) + I_Refract* lightIntensity);
				total += light_effect;
			}
			out_texture = mix(out_texture, color, ratio);
			if(total.x != 0.f || total.y != 0.f || total.z != 0.f)
			{
				out_texture *= total;
			}
				
		}
		FragColor = vec4(out_texture, 1.0);
	}
	
	else
		FragColor = vec4(vec3(0.5, 0.5, 0.5), 1.0);
}


vec3 reflect_mine(vec3 input_vec)
{
	vec3 uv = input_vec;
	vec3 mag = abs(input_vec);
	
	int is_x_positive = input_vec.x > 0 ? 1 : 0;
	int is_y_positive = input_vec.y > 0 ? 1 : 0;
	int is_z_positive = input_vec.z > 0 ? 1 : 0;
	
	float uc, vc;
	int index = -1;
	vec2 new_uv;
	
	if (is_x_positive == 1 && mag.x >= mag.y && mag.x >= mag.z)
	{
		index = 5;
		
		uc = -uv.z;
		vc = uv.y;
		
		new_uv.x = 1-0.5f * ((uc * -1.f) / mag.x + 1.0f);
		new_uv.y = 0.5f * (vc / mag.x + 1.0f);
	}
	
	else if (is_x_positive == 0 && mag.x >= mag.y && mag.x >= mag.z) 
	{
		index = 1;
		
		uc = uv.z;
		vc = uv.y;
		
		new_uv.x = 1-0.5 * ((uc * -1.f) / mag.x + 1.0);
		new_uv.y = 0.5 * (vc / mag.x + 1.0);
	}
	
	else if (is_y_positive == 1 && mag.y >= mag.x && mag.y >= mag.z) 
	{
		index = 3;
		
		uc = -uv.z;
		vc = -uv.x;
		
		new_uv.x = 0.5f * ((vc) / mag.y + 1.0f);
		new_uv.y = 1-0.5f * ((uc) / mag.y + 1.0f);
	}
	
	else if (is_y_positive == 0 && mag.y >= mag.x && mag.y >= mag.z) 
	{
		index = 4;
		
		uc = uv.z;
		vc = uv.x;
		
		new_uv.x = 0.5f * ((vc) / mag.y + 1.0f);
		new_uv.y = 0.5f * ((uc) / mag.y + 1.0f);
	}
	
	else if (is_z_positive == 1 && mag.z >= mag.y && mag.z >= mag.x) 
	{
		index = 0;
		
		uc = uv.x;
		vc = uv.y;
		
		new_uv.x = 1-0.5f * ((uc * -1.f) / mag.z + 1.0f);
		new_uv.y = 0.5f * (vc / mag.z + 1.0f);
	}
	
	else if (is_z_positive == 0 && mag.z >= mag.y && mag.z >= mag.x) 
	{
		index = 2;
		
		uc = -uv.x;
		vc = uv.y;
		
		new_uv.x = 1-0.5f * ((uc * -1.f) / mag.z + 1.0f);
		new_uv.y = 0.5f * (vc / mag.z + 1.0f);
	}
	return vec3(texture(env_box[index], new_uv));
}

vec3 GetDirLight(vec3 viewDir, vec3 lightDir, vec3 tex)
{
	vec3 ambient, diffuse, specular;
	vec3 reflectDir;
	vec3 out_color;
	for(int i = 0; i < num_sphere; ++i)
	{
		ambient = render_datas[i].light_color * .5 * tex;
		diffuse = max(dot(Normal, lightDir), 0.0) * render_datas[i].light_color * tex * .6;
		reflectDir = 2*(dot(Normal, lightDir)) * Normal - lightDir;
		specular = pow(max(dot(reflectDir, viewDir), 0.0), 5) * render_datas[i].light_color * tex * 2;
		
		out_color += (ambient + diffuse + specular);
	}
	
	return out_color;
}
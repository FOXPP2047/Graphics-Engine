/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: main.cpp
Purpose: main loop for obj loader
Language: c++
Platform: win64
Project: yongmin.cho_CS300_1
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.10.04
End Header --------------------------------------------------------*/
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include "include/stb_image.h"
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "Window.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "Camera.hpp"
#include "Sphere.hpp"

#include "render_data.h"
#include "PPMLoader.h"

#include <stb_image.h>
bool is_faceNormal = false;
bool is_vertexNormal = false;
bool is_wired = false;
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

Camera camera(glm::vec3(-5.f, 0.f, 2.f));
Camera fbo_camera(glm::vec3(0.f, 0.f, 0.f));
glm::vec3 lightPos(0.7f, 1.0f, 2.0f);

const int width = 1200, height = 800;

//For Combo Box
static int material_select = 0;
static bool is_wave = true;
static int toggle = 0;
static int style_idx = 0;
static int num_sphere = 2;
static int shader_idx = 0;
static int light_type_idx = 0;
static int light_color_idx = 0;
static float material = 1.0f;

static float constant = 1.0f;
static float linear = 0.09f;
static float quadratic = 0.032f;

static float inner = 12.5f;// = glm::cos(glm::radians(12.5f));
static float outer = 17.5f;// = glm::cos(glm::radians(17.5f));
static float falloff = 2.f;

static glm::vec3 ambient[MAX_LIGHT];// = { 123, 125, 200 };
static glm::vec3 diffuse[MAX_LIGHT];// = { 204, 122, 200 };
static glm::vec3 specular[MAX_LIGHT];// = { 45, 23, 123 };

static bool rotation = true;
static bool all_change = false;

static int cpu_shading = 1;
static int select_mapping = 0;
static int normal_pos = 0;

static glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
glm::vec3 light_direction;

static int scenario = 0;
//glm::vec4 color[MAX_LIGHT];
static bool is_add_phong = true;

const char* num_sphere_array[] = { "1 sphere", "2 spheres", "3 spheres", "4 spheres", "5 spheres", "6 spheres", "7 spheres", "8 spheres" };
const char* shader_file_name[] = { "PHONG_SHADING", "PHONG_LIGHTING", "BLINN_SHADING" };
const char* Light_type_name[] = { "POINT", "SPOT", "DIRECTIONAL" };
const char* Select_Light[] = { "Light0", "Light1", "Light2", "Light3", "Light4", "Light5", "Light6", "Light7", "Light8", 
                               "Light9", "Light10", "Light11", "Light12", "Light13", "Light14", "Light15", "Light16" };
const char* Scenario_type[] = { "Scenario 1", "Scenario 2", "Scenario 3" };
const char* mapping[] = { "CPU MAPPING", "GPU MAPPING" };
const char* mapping_method[] = { "CYLINDRICAL", "SPHERICAL", "CUBE" };
const char* normal_pos_select[] = { "NORMAL", "POSITION" };

float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

std::vector<glm::vec3> sphere_position;
render_data render_datas[MAX_LIGHT];
static bool set_up_fbo(unsigned int& fbo, GLuint* texture);
static void use_fbo(unsigned int fbo, int view_width, int view_height);

void initialize_stbi()
{
    stbi_set_flip_vertically_on_load(false);
}

void initialize_glad()
{
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");
}

void initialize_gl(int viewport_width, int viewport_height)
{
    glViewport(0, 0, viewport_width, viewport_height);
    glEnable(GL_DEPTH_TEST);
}

std::vector<glm::vec3> vertices;
unsigned int VAO;
void set_up_circle(float x, float y, float z, float radius)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    unsigned int v_VBO_draw;
    glGenBuffers(1, &v_VBO_draw);
    glBindBuffer(GL_ARRAY_BUFFER, v_VBO_draw);

    int num_sides = 360;
    int num_vertices = num_sides + 1;

    float double_size_pi = 2.0f * PI;

    for (int i = 0; i < num_vertices; ++i)
    {
        glm::vec3 temp;
        temp.x = x + (radius * cos(i * double_size_pi / num_sides));
        temp.y = y;
        temp.z = z + (radius * sin(i * double_size_pi / num_sides));
        
        vertices.push_back(temp);
    }

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES, 0, vertices.size() * 3);
}

void draw_circle()
{
    glBindVertexArray(VAO);

    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}

int main(int argc, char** argv)
{
    try
    {		
        for (int i = 0; i < MAX_LIGHT; ++i)
        {
            ambient[i] = { 0.1f, 0.1f, 0.1f };
            diffuse[i] = { 0.8f, 0.8f, 0.8f };
            specular[i] = { 1.0f, 1.0f, 1.0f };

            render_datas[i].ambient = ambient[i];
            render_datas[i].diffuse = diffuse[i];
            render_datas[i].specular = specular[i];
            render_datas[i].falloff = falloff;
            render_datas[i].inner = inner;
            render_datas[i].outer = outer;
            render_datas[i].light_color = glm::vec3(1.f, 1.f, 1.f);
        }

        camera.Pitch = 0.f;

        initialize_stbi();
        Window window(width, height, "Graphics Project");

        glfwSetCursorPosCallback(window.window_ptr, mouse_callback);
        glfwSetScrollCallback(window.window_ptr, scroll_callback);

        // tell GLFW to capture our mouse
        glfwSetInputMode(window.window_ptr, GLFW_CURSOR, GLFW_CURSOR);

        initialize_glad();
        initialize_gl(width, height);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window.window_ptr, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
        // Setup Dear ImGui style
        ImGui::StyleColorsLight();

        Shader shader("shaders/center_object.vert", "shaders/center_object.frag");
        Shader ball("shaders/ball.vert", "shaders/ball.frag");
        Shader sky_box_shader("shaders/sky_box.vert", "shaders/sky_box.frag");
        Sphere sphere;
        
        //Texture texture(uv_path);
        //texture.use();
        const std::string models[] = { "models/sphere.obj", "models/bunny.obj", "models/bunny_high_poly.obj", "models/cube.obj", "models/cube2.obj",
                                       "models/cup.obj", "models/lucy_princeton.obj", "models/quad.obj", "models/rhino.obj", "models/4sphere.obj",
                                       "models/sphere_modified.obj", "models/triangle.obj", "models/starwars1.obj"};
        
        std::vector<Model> model_container;
        Model model1, model2, model3, model4, model5, model6, model7, model8, model9, model10, model11, model12, model13;
        
        model1.load_model(models[0]);
        model1.set_up_VAO();
        model_container.push_back(model1);
        
        //For drawing Sky Box
        Model sky_box;
        sky_box.move = false;
        sky_box.load_model("models/sky_box.obj");
        sky_box.set_up_VAO();

		// skybox VAO
		float points[] = 
    {
		  -10.0f,  10.0f, -10.0f,
		  -10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,

		  -10.0f, -10.0f,  10.0f,
		  -10.0f, -10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f,  10.0f,
		  -10.0f, -10.0f,  10.0f,

		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,

		  -10.0f, -10.0f,  10.0f,
		  -10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f, -10.0f,  10.0f,
		  -10.0f, -10.0f,  10.0f,

		  -10.0f,  10.0f, -10.0f,
		   10.0f,  10.0f, -10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		  -10.0f,  10.0f,  10.0f,
		  -10.0f,  10.0f, -10.0f,

		  -10.0f, -10.0f, -10.0f,
		  -10.0f, -10.0f,  10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		  -10.0f, -10.0f,  10.0f,
		   10.0f, -10.0f,  10.0f
		};
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points, GL_STATIC_DRAW);

		GLuint skyboxVAO;
		glGenVertexArrays(1, &skyboxVAO);
		glBindVertexArray(skyboxVAO);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		GLuint sky_box_texture[6];
		std::string file_path[] = { "texture/c_front.tga", "texture/c_back.tga", "texture/c_left.tga", 
                                "texture/c_right.tga", "texture/c_up.tga", "texture/c_down.tga" };
		for (int i = 0; i < 6; ++i)
		{
			glGenTextures(1, &sky_box_texture[i]);
			glBindTexture(GL_TEXTURE_2D, sky_box_texture[i]);
			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load image, create texture and generate mipmaps
			int width_tex, height_tex, nrChannels;
			//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

			unsigned char* data;

			data = stbi_load(file_path[i].c_str(), &width_tex, &height_tex, &nrChannels, 0);

			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_tex, height_tex, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);
		}
    GLuint c_texture[6];
		unsigned int fbo = 0;
		set_up_fbo(fbo, c_texture);
    set_up_circle(-1.8f, -0.8f, 0.f, 4.65f);
            
        //PPMData* get_diff = PPMData::ReadPPM("texture/metal_roof_diff_512x512.ppm");
        //PPMData* get_spec = PPMData::ReadPPM("texture/metal_roof_spec_512x512.ppm");

        while(!glfwWindowShouldClose(window.window_ptr))
        {
            glfwPollEvents();
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window.window_ptr);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            // render your GUI
            //use_fbo(fbo, 512, 512);
            ImGui::Begin("DEBUG CONSOLE");

            if (ImGui::Combo("MODEL", &style_idx, "sphere\0bunny\0bunny_high_poly\0cube\0cube2\0cup\0lucy_princeton\0quad\0rihno\0 4sphere\0sphere_modified\0triangle\0starwars1\0"))
            {
                model_container.clear();

                switch (style_idx)
                {
                case 0: 
                    model1.move = true;
                    model1.load_model(models[0]); 
                    model1.set_up_VAO();
                    model_container.push_back(model1);
                    break;
                case 1: 
                    model2.move = true;
                    model2.load_model(models[1]);
                    model2.set_up_VAO();
                    model_container.push_back(model2);
                    break;
                case 2: 
                    model3.move = true;
                    model3.load_model(models[2]);
                    model3.set_up_VAO();
                    model_container.push_back(model3);
                    break;
                case 3: 
                    model4.move = true;
                    model4.load_model(models[3]); 
                    model4.set_up_VAO();
                    model_container.push_back(model4);
                    break;
                case 4: 
                    model5.move = true;
                    model5.load_model(models[4]); 
                    model5.set_up_VAO();                    
                    model_container.push_back(model5);
                    break;
                case 5:
                    model6.move = true;
                    model6.load_model(models[5]); 
                    model6.set_up_VAO();
                    model_container.push_back(model6);
                    break;
                case 6: 
                    model7.move = true;
                    model7.load_model(models[6]);
                    model7.set_up_VAO();
                    model_container.push_back(model7);
                    break;
                case 7: 
                    model8.move = true;
                    model8.load_model(models[7]);
                    model8.set_up_VAO();
                    model_container.push_back(model8);
                    break;
                case 8: 
                    model9.move = true;
                    model9.load_model(models[8]); 
                    model9.set_up_VAO();
                    model_container.push_back(model9);
                    break;
                case 9: 
                    model10.move = true;
                    model10.load_model(models[9]);
                    model10.set_up_VAO();
                    model_container.push_back(model10);
                    break;
                case 10: 
                    model11.move = true;
                    model11.load_model(models[10]);
                    model11.set_up_VAO();
                    model_container.push_back(model11);
                    break;
                case 11: 
                    model12.move = true;
                    model12.load_model(models[11]); 
                    model12.set_up_VAO();
                    model_container.push_back(model12);
                    break;
                case 12:
                    model13.move = true;
                    model13.load_model(models[12]);
                    model13.set_up_VAO();
                    model_container.push_back(model13);
                    break;
                }
                //model_container.push_back(sky_box);
            }

            ImGui::Checkbox("Add_Phong", &is_add_phong);
            if(ImGui::Combo("Select Render Type", &toggle, "reflection\0refraction\0combination"))
            {
                switch (toggle)
                {
                case 0:
                    toggle = 0;
                    break;
                case 1:
                    toggle = 1;
                    break;
                case 2:
                    toggle = 2;
                    is_wave = true;
                    is_add_phong = true;
                    break;
                }
            }

            ImGui::SliderFloat("Material Incidiences", &material, 0, 100);

            if (ImGui::Combo("Select Material", &material_select, "Air\0Hydrogen\0Water\0Olive Oil\0Ice\0Quartz\0Diamond\0Acrylic/Plexiglas/Lucite"))
            {
                switch (material_select)
                {
                case 0:
                    material = 1.000293f;
                    break;
                case 1:
                    material = 1.000132f;
                    break;
                case 2:
                    material = 1.333f;
                    break;
                case 3:
                    material = 1.47f;
                    break;
                case 4:
                    material = 1.31f;
                    break;
                case 5:
                    material = 1.46f;
                    break;
                case 6:
                    material = 2.42f;
                    break;
                case 7:
                    material = 1.49f;
                    break;
                }
            }
            ImGui::Checkbox("Show Vertex Normal", &is_vertexNormal);
            ImGui::Checkbox("Show Wire Frame", &is_wired);
            
            ImGui::Checkbox("Rotation", &rotation);

            ImGui::SliderInt("Number of Sphere(Light)", &num_sphere, 1, MAX_LIGHT);
            
            ImGui::SliderFloat("constant", &constant, 0, 1);
            ImGui::SliderFloat("linear", &linear, 0, 1);
            ImGui::SliderFloat("quadratic", &quadratic, 0, 1);
            
            if (ImGui::Combo("Choose Shader", &shader_idx, shader_file_name, IM_ARRAYSIZE(shader_file_name)))
            {
                switch (shader_idx)
                {
                case 0:
                    shader.set_shader_file("shaders/phong_shading.vert", "shaders/phong_shading.frag");
                    break;
                case 1:
                    shader.set_shader_file("shaders/phong_lighting.vert", "shaders/phong_lighting.frag");
                    break;
                case 2:
                    shader.set_shader_file("shaders/blinn.vert", "shaders/blinn.frag");
                    break;
                }
            }

            //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if(is_wired)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            transform = glm::translate(transform, glm::vec3(-0.9f, -1.f, -0.5f));

            static float add_angle = 0;

            if (ImGui::Combo("Select Scenario", &scenario, Scenario_type, IM_ARRAYSIZE(Scenario_type)))
            {
                srand(time(NULL));
                int type_random = 0;
                float color_x_random, color_y_random, color_z_random;
                switch (scenario)
                {
                case 0:
                    type_random = rand() % 3; //random 0 ~ 2
                    color_x_random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    color_y_random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    color_z_random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    for (int i = 0; i < num_sphere; ++i)
                    {
                        render_datas[i].type = static_cast<Light_type>(type_random);
                        render_datas[i].light_color = glm::vec3(color_x_random, color_y_random, color_z_random);
                    }
                    break;
                case 1:
                    color_x_random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    color_y_random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    color_z_random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    for (int i = 0; i < num_sphere; ++i)
                    {
                        type_random = rand() % 3; //random 0 ~ 2
                        render_datas[i].type = static_cast<Light_type>(type_random);
                        render_datas[i].light_color = glm::vec3(color_x_random, color_y_random, color_z_random);
                    }
                    break;
                case 2:
                    for (int i = 0; i < num_sphere; ++i)
                    {
                        type_random = rand() % 3; //random 0 ~ 2
                        color_x_random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                        color_y_random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                        color_z_random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                        render_datas[i].type = static_cast<Light_type>(type_random);
                        render_datas[i].light_color = glm::vec3(color_x_random, color_y_random, color_z_random);
                    }
                    break;
                }
            }



            use_fbo(fbo, 512, 512);
            glm::vec3 camrea_offset = camera.Position;
            float yaw_offset = camera.Yaw;
            float pitch_offset = camera.Pitch;
            camera.Position = glm::vec3(0.f, 0.f, 0.f);
            for (int i = 0; i < 6; ++i)
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                switch (i) 
                {
                case 0:
                    camera.Yaw = 180.f;
                    camera.Pitch = 0.f;
                    break;
                case 1:
                    camera.Yaw = 90.f;
                    camera.Pitch = 0.f;
                    break;
                case 2:
                    camera.Yaw = 0.f;
                    camera.Pitch = 0.f;
                    break;
                case 3:
                    camera.Yaw = 90.f;
                    camera.Pitch = 180.f;
                    break;
                case 4: //done
                    camera.Yaw = 90.f;
                    camera.Pitch = 90.f;
                    break;
                case 5: //done
                    camera.Yaw = -90.f;
                    camera.Pitch = -90.f;
                    break;
                }

                camera.updateCameraVectors();
                

                glm::mat4 projection = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 100.0f);
                glm::mat4 view = camera.GetViewMatrix();
                sky_box_shader.use();
                glm::mat4 identity_translate(1.0);
                sky_box_shader.setMat4("projection", projection);
                sky_box_shader.setMat4("view", glm::mat4(glm::mat3(view)));
                glm::mat4 s_model(1.0);// = glm::translate(identity_translate, camera.Position);
                sky_box_shader.setMat4("model", s_model);

                //glBindVertexArray(skyboxVAO);
                glEnable(GL_TEXTURE_2D);
                for (int i = 0; i < 6; ++i)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glBindTexture(GL_TEXTURE_2D, sky_box_texture[i]);
                    sky_box_shader.setTexture("sky_box[" + std::to_string(i) + "]", i);
                }
                sky_box_shader.setVec3("view_pos", camera.Position);

                glBindVertexArray(skyboxVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);
                //glDepthFunc(GL_LESS);

///////////////////////////////////////////////////////////////////////////////
                
                ball.use();
                sphere_position.clear();
                for (int i = 0; i < num_sphere; ++i)
                {
                    // get matrix's uniform location and set matrix
                    float degree = (i) * (360.f / num_sphere) + add_angle;
                    glm::vec3 pos = glm::vec3(cosf(glm::radians(degree)) * -5.f, 1.0f, sinf(glm::radians(degree)) * -5.f);
                    glm::mat4 rotate = glm::translate(transform, pos);

                    unsigned int transformLoc = glGetUniformLocation(ball.ID, "transform");

                    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

                    glm::mat4 projection = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 100.0f);
                    glm::mat4 view = camera.GetViewMatrix();
                    ball.setMat4("projection", projection);
                    ball.setMat4("view", view);
                    ball.setMat4("rotate", rotate);
                    ball.setVec3("objectColor", render_datas[i].light_color/*glm::vec3(0.2f, 0.2f, 0.3f)*/);
                    ball.setVec3("lightColor", glm::vec3(0.5f, 0.7f, 1.0f));
                    ball.setVec3("lightPos", lightPos);
                    glm::mat4 model = glm::mat4(1.0f);
                    ball.setMat4("model", model);
                    sphere_position.emplace_back(pos);

                    sphere.draw();
                }
                if (rotation)
                    add_angle += deltaTime * 20.f;

                ///////////////////////////////////////////////////////////////////////////////

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, c_texture[i], 0);
            }

            ///////////////////////////////////////////////////////////////////

            use_fbo(0, width, height);
            
            camera.Position = camrea_offset;
            camera.Yaw = yaw_offset;
            camera.Pitch = pitch_offset;
            camera.updateCameraVectors();

            //glDisable(GL_DEPTH_TEST);

            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float) height, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            sky_box_shader.use();
            glm::mat4 identity_translate(1.0);
            sky_box_shader.setMat4("projection", projection);
            sky_box_shader.setMat4("view", view);
            glm::mat4 s_model = glm::translate(identity_translate, camera.Position);
            sky_box_shader.setMat4("model", s_model);
            
            glBindVertexArray(skyboxVAO);
            glEnable(GL_TEXTURE_2D);
            for (int i = 0; i < 6; ++i)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, sky_box_texture[i]);
                sky_box_shader.setTexture("sky_box[" + std::to_string(i) + "]", i);
            }
            sky_box_shader.setVec3("view_pos", camera.Position);

            glBindVertexArray(skyboxVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            //glDepthFunc(GL_LESS);
            //glDepthFunc(GL_LESS);
            //sky_box.draw();
            //glCullFace(GL_BACK);
            //glEnable(GL_DEPTH_TEST);

            ball.use();
            sphere_position.clear();

            for (int i = 0; i < num_sphere; ++i)
            {
                // get matrix's uniform location and set matrix
                float degree = (i) * (360.f / num_sphere) + add_angle;
                glm::vec3 pos = glm::vec3(cosf(glm::radians(degree)) * -5.f, 1.0f, sinf(glm::radians(degree)) * -5.f);
                glm::mat4 rotate = glm::translate(transform, pos);

                unsigned int transformLoc = glGetUniformLocation(ball.ID, "transform");

                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
                glm::mat4 view = camera.GetViewMatrix();
                ball.setMat4("projection", projection);
                ball.setMat4("view", view);
                ball.setMat4("rotate", rotate);
                ball.setVec3("objectColor", render_datas[i].light_color/*glm::vec3(0.2f, 0.2f, 0.3f)*/);
                ball.setVec3("lightColor", glm::vec3(0.5f, 0.7f, 1.0f));
                ball.setVec3("lightPos", lightPos);
                glm::mat4 model = glm::mat4(1.0f);
                ball.setMat4("model", model);
                sphere_position.emplace_back(pos);

                sphere.draw();
            }
            if (rotation)
                add_angle += deltaTime * 20.f;
            
            shader.use();
            shader.setVec3("lightPos", lightPos);

            shader.setBool("is_object", true);
            shader.setInt("toggle", toggle);
            shader.setBool("waveLength", is_wave);
            shader.setBool("is_add_phong", is_add_phong);
            shader.setFloat("material", material);
            
            shader.setVec3("objectColor", objectColor);
            shader.setInt("num_sphere", num_sphere);
            shader.setVec3("viewPos", camera.Position);
            
            shader.setFloat("constant", constant);
            shader.setFloat("linear", linear);
            shader.setFloat("quadratic", quadratic);
            
            /*glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, get_diff->handle);
            glDisable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, get_spec->handle);
            glDisable(GL_TEXTURE_2D);*/

            if (ImGui::Button("RECOMPILE SHADER"))
            {
                switch (shader_idx)
                {
                case 0:
                    shader.set_shader_file("shaders/phong_shading.vert", "shaders/phong_shading.frag");
                    break;
                case 1:
                    shader.set_shader_file("shaders/phong_lighting.vert", "shaders/phong_lighting.frag");
                    break;
                case 2:
                    shader.set_shader_file("shaders/blinn.vert", "shaders/blinn.frag");
                    break;
                }
            }
            ImGui::Combo("Select Light", &light_color_idx, Select_Light, num_sphere);

            if (ImGui::Combo("Light Type", &light_type_idx, Light_type_name, IM_ARRAYSIZE(Light_type_name)))
                render_datas[light_color_idx].type = static_cast<Light_type>(light_type_idx);

            ImGui::SliderFloat("Inner", &inner, 0, 180);
            ImGui::SliderFloat("Outer", &outer, 0, 180);
            ImGui::SliderFloat("Falloff", &falloff, 0, 180);
            
            ImGui::Combo("MAPPING SLECT", &cpu_shading, mapping, IM_ARRAYSIZE(mapping));
            ImGui::Combo("CALCULATING MAPPING SLECT", &select_mapping, mapping_method, IM_ARRAYSIZE(mapping_method));
            ImGui::Combo("SELECT NORMAL & POSITION", &normal_pos, normal_pos_select, IM_ARRAYSIZE(normal_pos_select));

            ImGui::Checkbox("Change all Light", &all_change);
            if (all_change)
            {
                for (int i = 0; i < num_sphere; ++i)
                {
                    ImGui::SliderFloat("ambient", &render_datas[i].ambient.x, 0, 1);
                    ImGui::SliderFloat("diffuse", &render_datas[i].diffuse.x, 0, 1);
                    ImGui::SliderFloat("specular", &render_datas[i].specular.x, 0, 1);

                    ImGui::SliderFloat("Color R", &render_datas[i].light_color.x, 0, 1);
                    ImGui::SliderFloat("Color G", &render_datas[i].light_color.y, 0, 1);
                    ImGui::SliderFloat("Color B", &render_datas[i].light_color.z, 0, 1);
                }
            }
            else
            {
                ImGui::SliderFloat("ambient", &render_datas[light_color_idx].ambient.x, 0, 1);
                ImGui::SliderFloat("diffuse", &render_datas[light_color_idx].diffuse.x, 0, 1);
                ImGui::SliderFloat("specular", &render_datas[light_color_idx].specular.x, 0, 1);

                ImGui::SliderFloat("Color R", &render_datas[light_color_idx].light_color.x, 0, 1);
                ImGui::SliderFloat("Color G", &render_datas[light_color_idx].light_color.y, 0, 1);
                ImGui::SliderFloat("Color B", &render_datas[light_color_idx].light_color.z, 0, 1);
            }
            
            ImGui::End();

            for (GLint i = 0; i < num_sphere; ++i)
            {
                std::string number = std::to_string(i);
                shader.setInt("render_datas[" + number + "].type", light_type_idx);

                glUniform3f(glGetUniformLocation(shader.ID, ("render_datas[" + number + "].light_pos").c_str()), sphere_position[i].x, sphere_position[i].y, sphere_position[i].z);
                glUniform3f(glGetUniformLocation(shader.ID, ("render_datas[" + number + "].ambient").c_str()), render_datas[i].ambient.x, render_datas[i].ambient.y, render_datas[i].ambient.z);
                glUniform3f(glGetUniformLocation(shader.ID, ("render_datas[" + number + "].diffuse").c_str()), render_datas[i].diffuse.x, render_datas[i].diffuse.y, render_datas[i].diffuse.z);
                glUniform3f(glGetUniformLocation(shader.ID, ("render_datas[" + number + "].specular").c_str()), render_datas[i].specular.x, render_datas[i].specular.y, render_datas[i].specular.z);
                glUniform3f(glGetUniformLocation(shader.ID, ("render_datas[" + number + "].light_color").c_str()), render_datas[i].light_color.x, 
                                                                                                                   render_datas[i].light_color.y, 
                                                                                                                   render_datas[i].light_color.z);

                glUniform1f(glGetUniformLocation(shader.ID, ("render_datas[" + number + "].falloff").c_str()), falloff);
                glUniform1f(glGetUniformLocation(shader.ID, ("render_datas[" + number + "].outer").c_str()), glm::cos(glm::radians(outer)));
                glUniform1f(glGetUniformLocation(shader.ID, ("render_datas[" + number + "].inner").c_str()), glm::cos(glm::radians(inner)));

                glUniform3f(glGetUniformLocation(shader.ID, ("render_datas[" + number + "].direction").c_str()), camera.Front.x, camera.Front.y, camera.Front.z);
            }

            projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
            view = camera.GetViewMatrix();
            glm::mat4 basic = glm::mat4(1.0f);
            
            shader.setFloat("z_far", 20.f);
            shader.setFloat("z_near", 0.1f);
            shader.setVec3("I_fog", 0.5f, 0.5f, 0.5f);

            shader.setInt("material.diffuse", 0);
            shader.setInt("material.specular", 1);
            shader.setFloat("material.shininess", 16.f);

            glEnable(GL_TEXTURE_2D);
            for (int i = 0; i < 6; i++) 
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, c_texture[i]);
                shader.setTexture("env_box[" + std::to_string(i) + "]", i);
            }

            shader.setMat4("projection", projection);
            shader.setMat4("view", view);
            glm::mat4 m_model = glm::mat4(1.0f);
            //m_model = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()) * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            shader.setMat4("model", m_model);
            shader.setInt("cpu_shading", cpu_shading);
            shader.setInt("select_mapping", select_mapping);
            shader.setInt("normal_pos", normal_pos);
            

            // Draw Objects
            for (std::vector<Model>::iterator it = model_container.begin(); it != model_container.end(); ++it)
                it->draw();
            
            //Draw orbit
            //draw_circle();

            
            // Render dear imgui into screen
            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window.window_ptr);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        return EXIT_SUCCESS;
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << "Exception caught: " << err.what() << std::endl;
        return EXIT_FAILURE;
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(LOOKUP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(LOOKDOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LOOKLEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(LOOKRIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
}

 //glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // reversed since y-coordinates go from bottom to top

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

static bool set_up_fbo(unsigned int& fbo, GLuint* texture)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(6, texture);

	for (int i = 0; i < 6; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, texture[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture[i], 0);
	}

  //glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLuint depth_render_buffer;
	glGenRenderbuffers(1, &depth_render_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_render_buffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

static void use_fbo(unsigned int fbo, int view_width, int view_height)
{
  glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[UseFBO] Cannot switch to FBO - " << fbo << std::endl;
		std::cout << "[UseFBO] FBO not complete" << std::endl;
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, view_width, view_height);
}
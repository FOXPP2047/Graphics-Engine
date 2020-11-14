#include "MVP.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

MVP::MVP(/*const Shader &shader, float aspect_ratio, */) {}
    //shader(shader),
    /*model(glm::mat4(1.0f)),
    view(glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
    projection(glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f)),*/


void MVP::apply()
{
    update_model();

    /*shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);*/
}

void MVP::update_model()
{
    model = glm::rotate(glm::mat4(1.0f), 500 * (float)glfwGetTime() * glm::radians(0.0f), glm::vec3(1.0f, 1.0f, 0.0f));
}
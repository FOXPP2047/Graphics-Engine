/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Window.hpp
Purpose: header file of generating window
Language: c++
Platform: win64
Project: yongmin.cho_CS300_1
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.10.04
End Header --------------------------------------------------------*/
#pragma once

#include <GLFW/glfw3.h>
#include <string>

struct Window
{
    Window(int win_width, int win_height, const std::string &win_title);
    ~Window();

    int width, height;
    GLFWwindow *window_ptr;
private:
    static void framebuffer_size_callback(GLFWwindow *window, int new_width, int new_height);
};
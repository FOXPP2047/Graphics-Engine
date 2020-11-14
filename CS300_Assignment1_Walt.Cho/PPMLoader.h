/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PPMLoader.h
Purpose: Load PPM file
Language: c++
Platform: win64
Project: yongmin.cho_CS300_2
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.11.15
End Header --------------------------------------------------------*/
#pragma once

//#include <GL/glew.h>   // GLEW
#include <glm/glm.hpp> // GLM
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

using namespace std;

typedef unsigned char byte;
typedef byte* pbyte;

struct PPMData 
{
  struct RGB { unsigned char r, g, b; };

  glm::vec2 size = glm::vec2(0.0);
  std::vector<RGB> pixels;
  GLuint handle = 0;

  static PPMData* ReadPPM(const char* path)
  {
    ifstream f(path, ios::binary);
    if (f.fail())
    {
      cout << "Could not open file: " << path << endl;
      return nullptr;
    }

    PPMData* image = new PPMData;

    // get type of file
    eat_comment(f);
    int mode = 0;
    string s;
    f >> s;
    if (s == "P3")
      mode = 3;
    else if (s == "P6")
      mode = 6;

    // get w
    eat_comment(f);
    f >> image->size.x;

    // get h
    eat_comment(f);
    f >> image->size.y;

    // get bits
    eat_comment(f);
    int bits = 0;
    f >> bits;

    // load image data
    unsigned int resizing = image->size.x * image->size.y;
    image->pixels.resize(resizing);

    for (size_t i = 0; i < image->pixels.size(); i++)
    {
      int v;
      f >> v;
      image->pixels[i].r = unsigned char(v);
      f >> v;
      image->pixels[i].g = unsigned char(v);
      f >> v;
      image->pixels[i].b = unsigned char(v);
    }

    // close file
    f.close();
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &image->handle);
    glBindTexture(GL_TEXTURE_2D, image->handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->size.x, image->size.y, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &image->pixels[0]);

    return image;
  }

  static void eat_comment(ifstream& f)
  {
    char linebuf[1024];
    char ppp;
    while (ppp = char(f.peek()), ppp == '\n' || ppp == '\r')
      f.get();
    if (ppp == '#')
      f.getline(linebuf, 1023);
  }
};
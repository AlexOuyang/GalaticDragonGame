//
//  skybox.hpp
//  Project3_wedding_cake
//
//  Created by Chenxing Ouyang on 4/27/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

//#ifndef SHADER_HPP

#ifndef SKYBOX_H
#define SKYBOX_H

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "glm/ext.hpp" // print out glm matrices

class Skybox
{
private:
    GLuint loadCubemap(std::vector<const GLchar*> faces);
    
public:
    glm::mat4 toWorld;
    GLuint skyboxVAO, skyboxVBO;
    GLuint cubemapTexture;

    Skybox (float dimension, const GLchar* right, const GLchar* left, const GLchar* top, const GLchar* bottom, const GLchar* back, const GLchar* front);
    ~Skybox();

    void draw(GLuint shaderProgram);

};




#endif

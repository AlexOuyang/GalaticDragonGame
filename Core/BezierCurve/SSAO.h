//
//  SSAO.hpp
//  BezierCurve
//
//  Created by Yoon on 5/30/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#ifndef SSAO_h
#define SSAO_h

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
#include <random>


class SSAO{
private:
    
    //Lights
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    
    GLfloat lerp(GLfloat a, GLfloat b, GLfloat f);
    
public:
    SSAO() {}
    
    void bindSSAOLight(GLint ShaderProgram);
    void bindSSAO(GLint ShaderProgram);
    void setupLight(glm::vec3 light_Pos, glm::vec3 light_Color);
    void setupGBuffer(int width, int height);
};


#endif /* SSAO_hpp */


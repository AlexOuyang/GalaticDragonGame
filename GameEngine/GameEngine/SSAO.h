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
#include "OBJObject.h"


class SSAO
{
private:
    static GLint SSAOShaderProgram;
    static GLint SSAOBlurShaderProgram;
    static GLint SSAOGeometryShaderProgram;
    static GLint SSAOLightingShaderProgram;
    
    static void RenderQuad();
    
    //Linear Interpolation
    static GLfloat lerp(GLfloat a, GLfloat b, GLfloat f);

    static void bindSSAOLight(GLint ShaderProgram);
    static void bindSSAO(GLint ShaderProgram);
    static void setupLight(glm::vec3 light_Pos, glm::vec3 light_Color);
    static void setupGBuffer(int width, int height, int kernelSize = 64);
    
    /*================ SSAO Shaders =================*/
    static void drawSSAOGeometry(GLuint shaderProgram);
    static void drawSSAOTextures(GLuint shaderProgram);
    static void drawSSAOBlur(GLuint shaderProgram);
    static void drawSSAOLighting(GLuint shaderProgram, int draw_mode);
    
public:
    enum MODE
    {
        GRAY_SCALE_MODE,
        FOG_MODE,
    };
    
    // SSAO's default mode
    static MODE mode;
    
    //Light Properties
    static glm::vec3 lightPos;
    static glm::vec3 lightColor;
    
    // Create shaders and set up
    static void init(glm::vec3 light_pos, glm::vec3 light_color, MODE mode, bool use_material = false);
    
    // Add object to be drawn using SSAO
    static void add_obj(OBJObject * obj);
    
    // Delegate to object's draw function using SSAO shaders
    static void draw();
    
    // Delete the shaders
    static void delete_shaders();
    
    // Reinit the ssao shader
    static void re_init(MODE mode);
};


#endif /* SSAO_hpp */


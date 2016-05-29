//
//  Light.h
//  Project2_3DModel_Shader_Material
//
//  Created by Chenxing Ouyang on 4/20/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#ifndef Light_h
#define Light_h

#include <iostream>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>
#include "glm/ext.hpp" // print out glm matrices


class Light
{
public:
    enum LightToUse
    {
        NONE,
        SPOT_LIGHT,
        DIRECTIONAL_LIGHT,
        POINT_LIGHT,
    };
    static void setup();
    static void useLight(LightToUse light);
    static void bindDirectionalLightToShader(GLuint shaderProgram);
    static void bindPointLightToShader(GLuint shaderProgram);
    static void bindSpotLightToShader(GLuint shaderProgram);
    
    static void rotateDirectionalLight(float rotAngle, glm::vec3 rotAxis);
    
    static void orbitPointLight(float rotAngle, glm::vec3 rotAxis);
    static void changePointLightRadius(float delta_r);
    
    static void rotateSpotLight(float rotAngle, glm::vec3 rotAxis);
    static void orbitSpotLight(float rotAngle, glm::vec3 rotAxis);
    static void changeSpotLightRadius(float delta_r);
    static void changeSpotLightCutoff(float cutoff);
    static void changeSpotLightExponent(float exponent);
    
};


#endif /* Light_h */

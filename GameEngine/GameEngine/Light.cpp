//
//  Light.cpp
//  Project2_3DModel_Shader_Material
//
//  Created by Chenxing Ouyang on 4/20/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//


#include "Light.h"

struct DirectionalLight {
    bool on;
    glm::vec3 direction;
    glm::vec3 c_i; // c_i - light color intensity, a.k.a the color of the light
    GLfloat L_a; // Ambient coefficient, it is how much ambient influence the light emits. If this value is 1, the object's "inherent color" will shine through perfectly.
    GLfloat L_d; // Diffuse coefficient, it is how much of the color the light emits will be diffused.
    GLfloat L_s; //Specular coefficient, it is how much specular influence the light emit.
} directionalLight;


struct PointLight {
    bool on;
    glm::vec3 c_i; // c_i - light color intensity, a.k.a the color of the light
    glm::vec3 position;
    GLfloat L_a; // Ambient coefficient, it is how much ambient influence the light emits. If this value is 1, the object's "inherent color" will shine through perfectly.
    GLfloat L_d; // Diffuse coefficient, it is how much of the color the light emits will be diffused.
    GLfloat L_s; //Specular coefficient, it is how much specular influence the light emit.
} pointLight;


struct SpotLight {
    bool on;
    glm::vec3 c_i; // c_i - light color intensity, a.k.a the color of the light
    glm::vec3 position; // spot light position
    glm::vec3 direction; // Central axis of light source
    GLfloat cutoff; // Cutoff angle for the cone
    GLfloat exponent_coefficient; // spot light exponent
    GLfloat L_a; // Ambient coefficient, it is how much ambient influence the light emits. If this value is 1, the object's "inherent color" will shine through perfectly.
    GLfloat L_d; // Diffuse coefficient, it is how much of the color the light emits will be diffused.
    GLfloat L_s; //Specular coefficient, it is how much specular influence the light emit.
} spotLight;


void Light::setup()
{
    directionalLight.on = false;
    directionalLight.direction = glm::vec3(1.0f, 0.0f, 0.0f); // between the object and the camera
    directionalLight.c_i = glm::vec3(5.0f, 5.0f, 5.0f);
    directionalLight.L_a = 0.2f;
    directionalLight.L_d = 1.0f;
    directionalLight.L_s = 1.0f;
    
    pointLight.on = false;
    pointLight.position = glm::vec3(0.0f, 0.0f, 10.0f); // between the object and the camera
    pointLight.c_i = glm::vec3(10.0f, 10.0f, 10.0f);
    pointLight.L_a = 0.2f;
    pointLight.L_d = 1.0f;
    pointLight.L_s = 1.0f;
    
    spotLight.on = false;
    spotLight.direction = glm::vec3(0.0f, 0.0f, -1.0f); // between the object and the camera
    spotLight.position = glm::vec3(0.0f, 0.0f, 10.0f); // between the object and the camera
    spotLight.c_i = glm::vec3(10.0f, 10.0f, 10.0f);
    spotLight.cutoff = 30.0f;
    spotLight.exponent_coefficient = 1.0f;
    spotLight.L_a = 0.2f;
    spotLight.L_d = 1.0f;
    spotLight.L_s = 1.0f;
}


void Light::useLight(LightToUse light)
{
    spotLight.on = false;
    directionalLight.on = false;
    pointLight.on = false;
    
    switch (light) {
        case SPOT_LIGHT:
            spotLight.on = true;
            break;
        case DIRECTIONAL_LIGHT:
            directionalLight.on = true;
            break;
        case POINT_LIGHT:
            pointLight.on = true;
            break;
        case NONE:
        default:
            break;
    }
}


void Light::bindDirectionalLightToShader(GLuint shaderProgram)
{
    GLuint lightAttrID = glGetUniformLocation(shaderProgram, "directionalLight.on");
    glUniform1i(lightAttrID, directionalLight.on);
    lightAttrID = glGetUniformLocation(shaderProgram, "directionalLight.direction");
    glUniform3fv(lightAttrID, 1, &directionalLight.direction[0]);
    lightAttrID = glGetUniformLocation(shaderProgram, "directionalLight.c_i");
    glUniform3fv(lightAttrID, 1, &directionalLight.c_i[0]);
    lightAttrID = glGetUniformLocation(shaderProgram, "directionalLight.L_a");
    glUniform1f(lightAttrID, directionalLight.L_a);
    lightAttrID = glGetUniformLocation(shaderProgram, "directionalLight.L_d");
    glUniform1f(lightAttrID, directionalLight.L_d);
    lightAttrID = glGetUniformLocation(shaderProgram, "directionalLight.L_s");
    glUniform1f(lightAttrID, directionalLight.L_s);
}

void Light::bindPointLightToShader(GLuint shaderProgram)
{
    GLuint lightAttrID = glGetUniformLocation(shaderProgram, "pointLight.on");
    glUniform1i(lightAttrID, pointLight.on);
    lightAttrID = glGetUniformLocation(shaderProgram, "pointLight.position");
    glUniform3fv(lightAttrID, 1, &pointLight.position[0]);
    lightAttrID = glGetUniformLocation(shaderProgram, "pointLight.c_i");
    glUniform3fv(lightAttrID, 1, &pointLight.c_i[0]);
    lightAttrID = glGetUniformLocation(shaderProgram, "pointLight.L_a");
    glUniform1f(lightAttrID, pointLight.L_a);
    lightAttrID = glGetUniformLocation(shaderProgram, "pointLight.L_d");
    glUniform1f(lightAttrID, pointLight.L_d);
    lightAttrID = glGetUniformLocation(shaderProgram, "pointLight.L_s");
    glUniform1f(lightAttrID, pointLight.L_s);
}

void Light::bindSpotLightToShader(GLuint shaderProgram)
{
    GLuint lightAttrID = glGetUniformLocation(shaderProgram, "spotLight.on");
    glUniform1i(lightAttrID, spotLight.on);
    lightAttrID = glGetUniformLocation(shaderProgram, "spotLight.direction");
    glUniform3fv(lightAttrID, 1, &spotLight.direction[0]);
    lightAttrID = glGetUniformLocation(shaderProgram, "spotLight.position");
    glUniform3fv(lightAttrID, 1, &spotLight.position[0]);
    lightAttrID = glGetUniformLocation(shaderProgram, "spotLight.c_i");
    glUniform3fv(lightAttrID, 1, &spotLight.c_i[0]);
    lightAttrID = glGetUniformLocation(shaderProgram, "spotLight.cutoff");
    glUniform1f(lightAttrID, spotLight.cutoff);
    lightAttrID = glGetUniformLocation(shaderProgram, "spotLight.exponent_coefficient");
    glUniform1f(lightAttrID, spotLight.exponent_coefficient);
    lightAttrID = glGetUniformLocation(shaderProgram, "spotLight.L_a");
    glUniform1f(lightAttrID, spotLight.L_a);
    lightAttrID = glGetUniformLocation(shaderProgram, "spotLight.L_d");
    glUniform1f(lightAttrID, spotLight.L_d);
    lightAttrID = glGetUniformLocation(shaderProgram, "spotLight.L_s");
    glUniform1f(lightAttrID, spotLight.L_s);
}


/*============= Directional light =============*/


void Light::rotateDirectionalLight(float rotAngle, glm::vec3 rotAxis)
{
    auto rotationMat = glm::rotate(glm::mat4(1.0f), rotAngle / 180.0f * glm::pi<float>(), rotAxis);
    directionalLight.direction = glm::vec3(rotationMat * glm::vec4(directionalLight.direction, 0.0f));
    std::cout << "DirectionalLight rotation axis: " << glm::to_string(rotAxis) << " Degree: " << rotAngle << std::endl;
}

/*============= Point light =============*/

void Light::orbitPointLight(float rotAngle, glm::vec3 rotAxis)
{
    auto rotationMat = glm::rotate(glm::mat4(1.0f), rotAngle / 180.0f * glm::pi<float>(), rotAxis);
    pointLight.position = glm::vec3(rotationMat * glm::vec4(pointLight.position, 0.0f));
    std::cout << "PointLight orbit axis: " << glm::to_string(rotAxis) << " Degree: " << rotAngle << std::endl;
}

void Light::changePointLightRadius(float delta_r)
{
    float r_min = 1.0f;
    float r_magnitude = glm::length(pointLight.position);
    r_magnitude = ((r_magnitude + delta_r) < r_min) ? r_min : r_magnitude + delta_r;
    pointLight.position = r_magnitude * glm::normalize(pointLight.position);
    std::cout << "PointLight orbit sphere radius: " << r_magnitude << std::endl;
}


/*========== Spot light ==========*/

void Light::orbitSpotLight(float rotAngle, glm::vec3 rotAxis)
{
    auto rotationMat = glm::rotate(glm::mat4(1.0f), rotAngle / 180.0f * glm::pi<float>(), rotAxis);
    spotLight.position = glm::vec3(rotationMat * glm::vec4(spotLight.position, 0.0f));
    spotLight.direction = glm::vec3(0.0f) - spotLight.position;
    std::cout << "SpotLight orbit axis: " << glm::to_string(rotAxis) << " Degree: " << rotAngle << std::endl;
}

void Light::changeSpotLightRadius(float delta_r)
{
    float r_min = 1.0f;
    float r_magnitude = glm::length(spotLight.position);
    r_magnitude = ((r_magnitude + delta_r) < r_min) ? r_min : r_magnitude + delta_r;
    spotLight.position = r_magnitude * glm::normalize(spotLight.position);
    std::cout << "Spotlight orbit sphere radius: " << r_magnitude << std::endl;
}

void Light::rotateSpotLight(float rotAngle, glm::vec3 rotAxis)
{
    auto rotationMat = glm::rotate(glm::mat4(1.0f), rotAngle / 180.0f * glm::pi<float>(), rotAxis);
    spotLight.direction = glm::vec3(rotationMat * glm::vec4(spotLight.direction, 0.0f));
    std::cout << "PointLight rotation axis: " << glm::to_string(rotAxis) << " Degree: " << rotAngle << std::endl;
}


void Light::changeSpotLightCutoff(float cutoff)
{
    float cutoff_min = 0.0f;
    spotLight.cutoff = ((spotLight.cutoff + cutoff) < cutoff_min) ? cutoff_min : spotLight.cutoff + cutoff;
    std::cout << "Spotlight cutoff angle: " << spotLight.cutoff << std::endl;
}

void Light::changeSpotLightExponent(float exponent)
{
    float exponent_min = 1.0f;
    spotLight.exponent_coefficient = ((spotLight.exponent_coefficient + exponent) < exponent_min) ? exponent_min : spotLight.exponent_coefficient + exponent;
    std::cout << "Spotlight exponent coefficient: " << spotLight.exponent_coefficient << std::endl;
}






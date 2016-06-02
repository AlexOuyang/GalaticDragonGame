//
//  SSAO.cpp
//  BezierCurve
//
//  Created by Yoon on 5/30/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#include "SSAO.h"
#include "shader.h"


GLuint SSAO::ssaoFBO, SSAO::ssaoBlurFBO;
GLuint SSAO::gBuffer;
GLuint SSAO::gPositionDepth, SSAO::gNormal, SSAO::gAlbedo;
GLuint SSAO::rboDepth;
GLuint SSAO::ssaoColorBuffer, SSAO::ssaoColorBufferBlur;
GLuint SSAO::noiseTexture;

glm::vec3 SSAO::lightPos;
glm::vec3 SSAO::lightColor;

std::vector<glm::vec3> SSAO::ssaoKernel;

GLfloat SSAO::lerp(GLfloat a, GLfloat b, GLfloat f)
{
    return a + f * (b - a);
}

//setting up SSAOLight variables
void SSAO::bindSSAOLight(GLint ShaderProgram){
    glUniform1i(glGetUniformLocation(ShaderProgram, "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(ShaderProgram, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(ShaderProgram, "gAlbedo"), 2);
    glUniform1i(glGetUniformLocation(ShaderProgram, "ssao"), 3);
}

//setting up SSAO variables
void SSAO::bindSSAO(GLint ShaderProgram){
    glUniform1i(glGetUniformLocation(ShaderProgram, "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(ShaderProgram, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(ShaderProgram, "texNoise"), 2);
}

void SSAO::setupLight(glm::vec3 light_Pos, glm::vec3 light_Color){
    lightPos = light_Pos;
    lightColor = light_Color;
}

void SSAO::setupGBuffer(int width, int heigth, int kernelSize){
    
    // Set up G-Buffer
    // 3 textures:
    // 1. Positions + depth (RGBA)
    // 2. Color (RGB)
    // 3. Normals (RGB)
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    // - Position + linear depth color buffer
    glGenTextures(1, &gPositionDepth);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, heigth, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
    // - Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // - Albedo color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // - Create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, heigth);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "GBuffer Framebuffer not complete!" << std::endl;
    
    // Also create framebuffer to hold SSAO processing stage
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    // - SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, heigth, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // - and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, heigth, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Sample kernel
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (GLuint i = 0; i < kernelSize; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / (float)kernelSize;
        
        // Scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    
    
    
    // Noise texture
    std::vector<glm::vec3> ssaoNoise;
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}


/*=============  Public methods =============*/


GLint SSAO::SSAOShaderProgram;
GLint SSAO::SSAOBlurShaderProgram;
GLint SSAO::SSAOGeometryShaderProgram;
GLint SSAO::SSAOLightingShaderProgram;

void SSAO::init(int width, int height)
{
    SSAOShaderProgram = LoadShaders("./shaders/shader_SSAO.vert",
                                    "./shaders/shader_SSAO.frag");
    SSAOBlurShaderProgram = LoadShaders("./shaders/shader_SSAO_blur.vert",
                                        "./shaders/shader_SSAO_blur.frag");
    SSAOGeometryShaderProgram = LoadShaders("./shaders/shader_SSAO_geometry.vert",
                                            "./shaders/shader_SSAO_geometry.frag");
    SSAOLightingShaderProgram = LoadShaders("./shaders/shader_SSAO_lighting.vert",
                                            "./shaders/shader_SSAO_lighting.frag");
    
    glUseProgram(SSAOLightingShaderProgram);
    SSAO::bindSSAOLight(SSAOLightingShaderProgram);
    glUseProgram(SSAOShaderProgram);
    SSAO::bindSSAO(SSAOShaderProgram);
    
    //setup light properties
    SSAO::setupLight(lightPos, lightColor);
    SSAO::setupGBuffer(width, height);
}


void SSAO::delete_shaders()
{
    glDeleteProgram(SSAOShaderProgram);
    glDeleteProgram(SSAOBlurShaderProgram);
    glDeleteProgram(SSAOGeometryShaderProgram);
    glDeleteProgram(SSAOLightingShaderProgram);
}


// Keeps track of all the objects to draw
std::vector<OBJObject *> objects_to_draw;

void SSAO::add_obj(OBJObject * obj)
{
    objects_to_draw.push_back(obj);
}


void SSAO::draw()
{
    objects_to_draw[0]->drawSSAOGeometry(SSAOGeometryShaderProgram);
    
    //draw all objects
    for(int i = 0; i < objects_to_draw.size(); i++)
        objects_to_draw[i]->drawSSAO(SSAOGeometryShaderProgram);
    

    //use other shaders
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    objects_to_draw[0]->drawSSAOTextures(SSAOShaderProgram);
    objects_to_draw[0]->drawSSAOBlur(SSAOBlurShaderProgram);
    objects_to_draw[0]->drawSSAOLighting(SSAOLightingShaderProgram, 1);
}


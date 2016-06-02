//
//  SSAO.cpp
//  BezierCurve
//
//  Created by Yoon on 5/30/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#include "SSAO.h"
#include "shader.h"
#include "Window.h"

// Define static variables
glm::vec3 SSAO::lightPos;
glm::vec3 SSAO::lightColor;


//Light Properties
GLuint ssaoFBO, ssaoBlurFBO;
GLuint gBuffer;
GLuint gPositionDepth, gNormal, gAlbedo;
GLuint rboDepth;
GLuint ssaoColorBuffer, ssaoColorBufferBlur;
GLuint noiseTexture;

std::vector<glm::vec3> ssaoKernel;

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



/*================ SSAO Shaders =================*/


void SSAO::drawSSAOGeometry(GLuint shaderProgram){
    
    // 1. Geometry Pass: render scene's geometry/color data into gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shaderProgram);
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &Window::V[0][0]);
}


void SSAO::drawSSAOTextures(GLuint shaderProgram){
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderProgram);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    // Send kernel + rotation
    
    for (GLuint i = 0; i < ssaoKernel.size(); ++i)
        glUniform3fv(glGetUniformLocation(shaderProgram, ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SSAO::drawSSAOBlur(GLuint shaderProgram){
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SSAO::drawSSAOLighting(GLuint shaderProgram, int draw_mode){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glActiveTexture(GL_TEXTURE3); // Add extra SSAO texture to lighting pass
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    // Also send light relevant uniforms
    glm::vec3 lightPosView = glm::vec3(Window::V * glm::vec4(SSAO::lightPos, 1.0));
    glUniform3fv(glGetUniformLocation(shaderProgram, "light.Position"), 1, &lightPosView[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "light.Color"), 1, &SSAO::lightColor[0]);
    // Update attenuation parameters
    //    const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
    const GLfloat linear = 0.09;
    const GLfloat quadratic = 0.032;
    glUniform1f(glGetUniformLocation(shaderProgram, "light.Linear"), linear);
    glUniform1f(glGetUniformLocation(shaderProgram, "light.Quadratic"), quadratic);
    glUniform1i(glGetUniformLocation(shaderProgram, "draw_mode"), draw_mode);
    RenderQuad();
}



/*========= Used for rendering quad ===========*/

GLuint quadVAO = 0;
GLuint quadVBO;

void SSAO::RenderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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

// Clean up shaders
void SSAO::delete_shaders()
{
    glDeleteProgram(SSAOShaderProgram);
    glDeleteProgram(SSAOBlurShaderProgram);
    glDeleteProgram(SSAOGeometryShaderProgram);
    glDeleteProgram(SSAOLightingShaderProgram);
}


// Keeps track of all the objects to draw
std::vector<OBJObject *> objects_to_draw;

// Add object to be drawn using SSAO
void SSAO::add_obj(OBJObject * obj)
{
    objects_to_draw.push_back(obj);
}


// Delegate to object's draw function using SSAO shaders
void SSAO::draw()
{
    drawSSAOGeometry(SSAOGeometryShaderProgram);
    
    //draw all objects
    for(int i = 0; i < objects_to_draw.size(); i++)
        objects_to_draw[i]->drawSSAO(SSAOGeometryShaderProgram);
    
    //use other shaders
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    drawSSAOTextures(SSAOShaderProgram);
    drawSSAOBlur(SSAOBlurShaderProgram);
    drawSSAOLighting(SSAOLightingShaderProgram, 1);
}





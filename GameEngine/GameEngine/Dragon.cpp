//
//  Dragon.cpp
//  GameEngine
//
//  Created by Chenxing Ouyang on 6/4/16.
//  Copyright © 2016 CSE167. All rights reserved.
//
#include "glm/ext.hpp" // print out glm matrices
#include "OBJObject.h"
#include "Window.h"

#include "Dragon.h"

/*================== Dragon Wing ==================*/

DragonWing::DragonWing(const char* filepath, int type):
OBJObject(filepath),
type(type)
{
    this->speed = 3.0f;
    this->angle = 0;
    if (type == 0) this->direction = 1;
    else if (type == 1) this->direction = -1;
}


void DragonWing::translationWing(float x, float y, float z)
{
    this->transform.position += glm::vec3(x, y, z);
    this->translate(x,y,z);
}


void DragonWing::update()
{
    if (this->type == 0) // left wing
    {
        if (this->angle > 10) direction = -1;
        if (this->angle < -60) direction = 1;
        
        float deg = direction * speed;
        this->angle += deg;
        if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
        auto rotationMat = glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(0,1,0));
        
        this->translate(-this->transform.position);
        
        this->translate(0.05f, -0.5f, 0);
        this->toWorld = rotationMat * this->toWorld;
        this->translate(-0.05f, 0.5f, 0);
        
        this->translate(this->transform.position);
        
        //        std::cout << angle << std::endl;
    }
    else if (this->type == 1) // right wing
    {
        if (this->angle > 60) direction = -1;
        if (this->angle < -10) direction = 1;
        
        float deg = direction * speed;
        this->angle += deg;
        if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
        auto rotationMat = glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(0,1,0));
        
        this->translate(-this->transform.position);
        
        this->translate(-0.05f, 0, 0);
        this->toWorld = rotationMat * this->toWorld;
        this->translate(0.05f, 0, 0);
        
        this->translate(this->transform.position);
    }
}

// When falling down, the dragon's wing spins faster and faster like a piece leaf losting in the ocean....
void DragonWing::falling()
{
    this->speed += 0.02f;

    // When falling ,flappying speed is faster
    if (this->type == 0) // left wing
    {
//        if (this->angle > 60) direction = 0;
//        if (this->angle < -60) direction = 1;
        
        float deg = direction * speed;
        this->angle += deg;
        if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
        auto rotationMat = glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(0,1,0));
        
//        this->translate(-this->transform.position);
        
//        this->translate(0.05f, -0.5f, 0);
        this->toWorld = rotationMat * this->toWorld;
//        this->translate(-0.05f, 0.5f, 0);
        
//        this->translate(this->transform.position);
    }
    else if (this->type == 1) // right wing
    {

//        if (this->angle > 60) direction = -1;
//        if (this->angle < -60) direction = 0;
        
        float deg = direction * speed;
        this->angle += deg;
        if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
        auto rotationMat = glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(0,1,0));
        
//        this->translate(-this->transform.position);
        
//        this->translate(-0.05f, 0, 0);
        this->toWorld = rotationMat * this->toWorld;
//        this->translate(0.05f, 0, 0);
        
//        this->translate(this->transform.position);
    }
}


/*=============== Dragon ==================*/


Dragon::Dragon(const char* dragon_body_path, const char* dragon_left_wing_path, const char* dragon_right_wing_path)
{
    speed = 0.1f;
    rotationSpeed = 2.0f;
    
    body = new OBJObject(dragon_body_path);
    leftWing = new DragonWing(dragon_left_wing_path, 0);
    rightWing = new DragonWing(dragon_right_wing_path, 1);
    
    // Create bonding box for dragon body
    body->createBoundingBox();
    body->getBoundingBox()->scale(glm::vec3(1.2f, 0.3f, 0.25f));
    body->getBoundingBox()->translate(glm::vec3(0, 0, 0.1f));
    body->tag = "dragon";
    
    
    // Set up the dragon model
    body->scale(1.2f);
    body->translate(0, -0.04f, 0);
    leftWing->scale(0.8f);
    rightWing->scale(0.8f);
    //    hero->translate(-0.015f, 0, 0);
    leftWing->translate(0.35f, 0.24f, -0.05f);
    rightWing->translate(-0.35f, 0.24f, -0.05f);
    
    // Rotate the dragon body parts since blender exported orientation is off
    body->rotate(90.0f,glm::vec3(-1.0f,0.0f,0.0f));
    body->rotate(180.0f,glm::vec3(0.0f,1.0f,0.0f));
    
    leftWing->rotate(90.0f,glm::vec3(-1.0f,0.0f,0.0f));
    leftWing->rotate(180.0f,glm::vec3(0.0f,1.0f,0.0f));
    
    rightWing->rotate(90.0f,glm::vec3(-1.0f,0.0f,0.0f));
    rightWing->rotate(180.0f,glm::vec3(0.0f,1.0f,0.0f));
}

void Dragon::translate(float x, float y, float z)
{
    body->translate(x, y, z);
    leftWing->translationWing(x, y, z);
    rightWing->translationWing(x, y, z);
}


void Dragon::flap()
{
    leftWing->update();
    rightWing->update();
}

Dragon::~Dragon()
{
    delete body;
    delete leftWing;
    delete rightWing;
}

void Dragon::update(bool moveLeft, bool moveRight, bool moveUp, bool moveDown)
{
    flap();
    
    if(moveLeft)
    {
        this->translate(-speed, 0, 0);
        Window::rotate_cam(rotationSpeed, glm::vec3(0, 1, 0));
    }
    if(moveRight)
    {
        this->translate(speed, 0, 0);
        Window::rotate_cam(-rotationSpeed, glm::vec3(0, 1, 0));
    }
    if(moveUp)
    {
        this->translate(0, 0, speed);
    }
    if(moveDown)
    {
        this->translate(0, 0,-speed);
    }
    
    body->updateBoundingBox();
}

void Dragon::falling()
{
    leftWing->falling();
    leftWing->falling();
    this->translate(0, 0,-speed);
    body->updateBoundingBox();
}







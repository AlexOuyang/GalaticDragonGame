//
//  Dragon.cpp
//  GameEngine
//
//  Created by Chenxing Ouyang on 6/4/16.
//  Copyright © 2016 CSE167. All rights reserved.
//
#include "glm/ext.hpp" // print out glm matrices
#include "OBJObject.h"
#include "Dragon.h"


DragonWing::DragonWing(const char* filepath, int type):
OBJObject(filepath),
type(type)
{
    this->speed = 2.0f;
    this->angle = 0;
    if (type == 0) this->direction = 1;
    else if (type == 1) this->direction = -1;
}


void DragonWing::translationWing(float x, float y, float z)
{
    this->transform.position += glm::vec3(x, y, z);
    std::cout << glm::to_string(this->transform.position) << std::endl;
    this->translate(x,y,z);
}


void DragonWing::update()
{
    if (this->type == 0) // left wing
    {
        if (this->angle > 20) direction = -1;
        if (this->angle < -50) direction = 1;
        
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
        if (this->angle > 50) direction = -1;
        if (this->angle < -20) direction = 1;
        
        float deg = direction * speed;
        this->angle += deg;
        if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
        auto rotationMat = glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(0,1,0));
        
        this->translate(-this->transform.position);

        this->translate(-0.05f, 0, 0);
        this->toWorld = rotationMat * this->toWorld;
        this->translate(0.05f, 0, 0);
        
        this->translate(this->transform.position);

//        std::cout << angle << std::endl;
    }
}
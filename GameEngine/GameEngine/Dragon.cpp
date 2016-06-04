//
//  Dragon.cpp
//  GameEngine
//
//  Created by Chenxing Ouyang on 6/4/16.
//  Copyright © 2016 CSE167. All rights reserved.
//

#include "OBJObject.h"
#include "Dragon.h"


void DragonWing::update()
{
    float deg = 1;
    this->angle += deg;
    if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
    auto rotationMat = glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(0,0,1));
    this->toWorld = this->toWorld * rotationMat;
}
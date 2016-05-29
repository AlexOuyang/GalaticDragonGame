//
//  Node.cpp
//  Project3_wedding_cake
//
//  Created by Chenxing Ouyang on 5/3/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//
#include "OBJObject.h"

#include "Node.h"


/*============= Group ===============*/

void Group::addChild(Node* n)
{
    children.push_back(n);
}

void Group::removeChild(Node* n)
{
    children.remove(n);
}

/*============= MatrixTransform ===============*/

void MatrixTransform::update(glm::mat4 C)
{
    for (std::list<Node*>::const_iterator ci = children.begin(); ci != children.end(); ++ci)
    {
        (*ci)->update(C*M);
//        (*ci)->draw();
    }
}

void MatrixTransform::draw()
{
    for (std::list<Node*>::const_iterator ci = children.begin(); ci != children.end(); ++ci)
    {
        (*ci)->draw();
    }
}

void MatrixTransform::update()
{
    update(glm::mat4(1.0));
}


/*============= Geode ===============*/

void Geode::update(glm::mat4 C)
{
    M = C * M;
}

void Geode::draw()
{
    toDraw->toWorld = M;
    toDraw->draw(shaderProgram);
    
    if (drawBear)
    {
        bear->toWorld = M * bearLocalTransform;
        bear->draw(shaderProgram);
    }
}

void Geode::addBear(OBJObject * bear, glm::mat4 bearLocalTransform)
{
    this->bear = bear;
    this->bearLocalTransform = bearLocalTransform;
    drawBear = true;
}








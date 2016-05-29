//
//  Node.h
//  Project3_wedding_cake
//
//  Created by Chenxing Ouyang on 5/3/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#ifndef Node_h
#define Node_h

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
#include "glm/ext.hpp" // print out glm matrices
#include <list>


class Node
{
public:
    virtual void update(glm::mat4 C) = 0;
    virtual void draw() = 0;
};


class Group : public Node
{
public:
    std::list<Node*> children;
    
    void addChild(Node* n);
    
    void removeChild(Node* n);
    
    
    // Loop through all Node* in children and call update(C) passing in glm::mat4 C as an argument
    virtual void update(glm::mat4 C) = 0;
    
    virtual void draw() = 0;
};


class MatrixTransform : public Group
{
public:
    glm::mat4 M;
    
    MatrixTransform(glm::mat4 m):M(m) {}
    
    // Loop through all Node* in children and call update(C) passing in glm::mat4 C as an argument
    // Multiplies M with C (C*M) and passes resulting glm::mat4 down to children
    void update(glm::mat4 C);
    
    void update();

    void draw();
};


class Geode : public Node
{
private:
    OBJObject * bear;
    bool drawBear;
    glm::mat4 bearLocalTransform;
public:
    OBJObject * toDraw;
    GLint shaderProgram;
    glm::mat4 M;
    
    Geode(OBJObject * obj, GLint shader):
    bear(nullptr),
    drawBear(false),
    bearLocalTransform(glm::mat4(1)),
    toDraw(obj),
    shaderProgram(shader),
    M(glm::mat4(1.0f))
    {}
    
    ~Geode() {
        delete toDraw;
    }
    
    // Our Geode now has to hold the matrix passed down to it
    void update(glm::mat4 C);
    
    // Call draw on the toDraw pointer
    void draw();
    
    // Add bear into the pod.
    void addBear(OBJObject * bear, glm::mat4 bearLocalTransform);
};

#endif /* Node_h */

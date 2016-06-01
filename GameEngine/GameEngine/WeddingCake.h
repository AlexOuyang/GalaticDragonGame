//
//  WeddingCake.h
//  Project3_wedding_cake
//
//  Created by Chenxing Ouyang on 5/5/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#ifndef WeddingCake_h
#define WeddingCake_h

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

class WeddingCake
{
private:
    // update pingpong value and angle
    void preUpdate();
    // Returns a propeller with extendable 3 pods based on the index of the propeller
    MatrixTransform * createPropeller(int propeller_index);
    // Returning a spinning level 1 propeller with 3 extendable small propeller based on the index of the level
    MatrixTransform * createLevel(int level_index);
    // Assembles the wedding cake with a center post and a base
    MatrixTransform * createWeddingCake();
    
public:
    MatrixTransform * weddingCake;
    OBJObject * bearObj;
    WeddingCake(GLuint shaderProgram, OBJObject * cylinder, OBJObject * pod, OBJObject * bearObj);
    ~WeddingCake();
    
    void update();
    void draw();
    void addBearToPod(int podIndex);
};

#endif /* WeddingCake_h */

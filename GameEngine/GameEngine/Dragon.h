//
//  Dragon.h
//  GameEngine
//
//  Created by Chenxing Ouyang on 6/4/16.
//  Copyright © 2016 CSE167. All rights reserved.
//

#ifndef Dragon_h
#define Dragon_h

#include <stdio.h>

class DragonWing : public OBJObject
{
private:
    int type; // 0 for left wing, 1 for right wing
    float speed; // Dragon wing's flapping speed
    float angle;
    int direction;
    
public:
    DragonWing(const char* filepath, int type);
    
    void update();
    void falling();
    void translationWing(float x, float y, float z);
};


class Dragon
{
private:
    void translate(float x, float y, float z);
    void flap();
    
public:
    float speed; // Dragon's flying speed
    float rotationSpeed; // Dragon's rotating speed
    OBJObject * body;
    DragonWing * leftWing;
    DragonWing * rightWing;
    
    Dragon(const char* dragon_body_path, const char* dragon_left_wing_path, const char* dragon_right_wing_path);
    ~Dragon();
    
    // Called to move the dragon
    void update(bool moveLeft, bool moveRight, bool moveUp, bool moveDown);
    // Called to make the dragon fall down after it gets hit
    void falling();
};



#endif /* Dragon_h */

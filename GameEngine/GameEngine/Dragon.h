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
    float speed;
    float angle;
    int direction;
    
public:
    DragonWing(const char* filepath, int type);
    
    void update();
    void translationWing(float x, float y, float z);
};


class Dragon
{
public:
    OBJObject * hero;
    DragonWing * leftWing;
    DragonWing * rightWing;
    
};



#endif /* Dragon_h */

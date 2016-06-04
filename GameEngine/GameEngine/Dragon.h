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
    float angle;
    
public:
    DragonWing(const char* filepath):
    OBJObject(filepath),
    angle(0)
    {}
    
    void update();
};



#endif /* Dragon_h */

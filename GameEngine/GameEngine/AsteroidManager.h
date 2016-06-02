
#ifndef AsteroidManager_h
#define AsteroidManager_h

#include <stdio.h>
#include <iostream>
#include "OBJObject.h"


class Asteroid : public OBJObject
{
    
};


class AsteroidManager
{
public:
    std::vector<Asteroid*> asteroids;
    
    
};


#endif /* AsteroidManager_h */

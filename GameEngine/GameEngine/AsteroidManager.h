
#ifndef AsteroidManager_h
#define AsteroidManager_h

#include <stdio.h>
#include <iostream>


class Asteroid : public OBJObject
{
private:
    glm::vec3 velocity;
public:
    Asteroid(const char* filepath):
    OBJObject(filepath)
    {}
};


class AsteroidGroup
{
private:
    int num_of_asteroids;
    float bound_top;
    float bound_down;
    float bound_left;
    float bound_right;
    
public:
    std::vector<Asteroid*> asteroids;
    
    AsteroidGroup(int num_of_asteroids, float bound_top, float bound_down,
                  float bound_left, float bound_right);
    
    void addAsteroids(float offset);
    
    void moveAsteroids();
};


#endif /* AsteroidManager_h */

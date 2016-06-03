
#ifndef AsteroidManager_h
#define AsteroidManager_h

#include <stdio.h>
#include <iostream>
#include <stdlib.h>  //rand
#include <time.h>    //for setting seed of rand()

class Asteroid : public OBJObject
{
public:
    Asteroid(const char* filepath):
    OBJObject(filepath)
    {}
    
    glm::vec3 velocity;
    glm::vec4 original_pos;
};


class AsteroidGroup
{
private:
    int num_of_asteroids;
    float bound_top;
    float bound_down;
    float bound_left;
    float bound_right;
    float bound_z_pos;
    float bound_z_neg;
    
    //get a random float between max and min values
    float randFloat(float max, float min);
    
public:
    static int num_of_asteroids_passed;
    
    std::vector<Asteroid*> asteroids;
    
    AsteroidGroup(int num_of_asteroids, float bound_top, float bound_down,
                  float bound_left, float bound_right, float bound_z_pos, float bound_z_neg);
    
    void addAsteroids(glm::vec4 new_pos);
    
    void update();
    //done every update()
    void moveAsteroids();
    void checkBounds();
    
    //randomize the position of the asteroids
    glm::vec4 randomize_position();
    void reset(Asteroid* asteroid);
};


#endif /* AsteroidManager_h */

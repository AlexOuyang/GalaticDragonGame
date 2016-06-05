
#ifndef AsteroidManager_h
#define AsteroidManager_h

#include <stdio.h>
#include <iostream>
#include <stdlib.h>  //rand
#include <time.h>    //for setting seed of rand()

class Asteroid : public OBJObject
{
public:
    glm::vec3 velocity;
    float rotationVelocity;
    
    Asteroid(const char* filepath):
    OBJObject(filepath)
    {}

};


/**
 * Create an asteroid group in a 3d space
 */
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
    float speed_multiplier;
    
    // Add an asteroid to group
    void addAsteroid(glm::vec4 position, float scale);

    // Get a random float between max and min values
    float randFloat(float max, float min);
    
    // Give a randomized the position of the asteroid
    glm::vec4 randomPosition();
    
    // Give a randomized the velocity for the asteroid
    glm::vec3 randomVelocity();
    
    // Reset asteroid if out of bound
    void reset(Asteroid* asteroid);
    
    // Done in every update() to move asteroids
    void moveAsteroids();
    
    // Done in every update() to check if asteroids out of bound, if true the reset them.
    void checkBounds();
    
    // Called in update() to speed up asteroids over time based on the total num of asteroids passed
    void levelUp();
    
public:
    int numOfAsteroidsPassed;
    
    std::vector<Asteroid*> asteroids;
    
    AsteroidGroup(int num_of_asteroids, float bound_top, float bound_down,
                  float bound_left, float bound_right, float bound_z_pos, float bound_z_neg);
    ~AsteroidGroup();
    
    void update();

};


#endif /* AsteroidManager_h */

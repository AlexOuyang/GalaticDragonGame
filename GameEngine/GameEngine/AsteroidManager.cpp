
#include "OBJObject.h"
#include "AsteroidManager.h"


AsteroidGroup::AsteroidGroup(int num_of_asteroids, float bound_top, float bound_down,
                             float bound_left, float bound_right, float bound_z_pos, float bound_z_neg):
num_of_asteroids(num_of_asteroids),
bound_top(bound_top),
bound_down(bound_down),
bound_left(bound_left),
bound_right(bound_right),
bound_z_pos(bound_z_pos),
bound_z_neg(bound_z_neg),
speed_multiplier(0.8f),
numOfAsteroidsPassed(0)
{
    //setting random seed
    srand((unsigned int)time(NULL));
    
    for(int i = 0; i < num_of_asteroids; i ++)
    {
        addAsteroid(randomPosition(), randFloat(0.2f, 2.0f));
    }
}

AsteroidGroup::~AsteroidGroup()
{
    for (int i = 0; i < asteroids.size(); i++)
        delete asteroids[i];
}


void AsteroidGroup::addAsteroid(glm::vec4 position, float scale)
{
    Asteroid * asteroid;
    // Chose the asteroid based on scale
    if (scale < 0.8f)
    {
        asteroid = new Asteroid("../../Models/Asteroid/asteroid_simple_1.obj");
    }
    else
    {
        if (asteroids.size() % 3 == 0)
            asteroid = new Asteroid("../../Models/Asteroid/asteroid_complex_1.obj");
        else if (asteroids.size() % 3 == 1)
            asteroid = new Asteroid("../../Models/Asteroid/asteroid_complex_2.obj");
        else
            asteroid = new Asteroid("../../Models/Asteroid/asteroid_complex_3.obj");
    }
    asteroid->material.k_a = glm::vec3(1);
    asteroid->material.k_d = glm::vec3(1);
    asteroid->material.k_s = glm::vec3(1);
    asteroid->material.shininess = 0;
    asteroid->setPosition(position);
    asteroid->scale(scale);
    asteroid->velocity = randomVelocity();
    asteroids.push_back(asteroid);
}

// Done in every update() to move asteroids
void AsteroidGroup::moveAsteroids()
{
    for(int i = 0; i < asteroids.size(); i ++){
        asteroids[i]->translate(asteroids[i]->velocity);
    }
}

// Done in every update() to check if asteroids out of bound, if true the reset them.
void AsteroidGroup::checkBounds(){
    for(int i = 0; i < asteroids.size(); i ++){
        if(asteroids[i]->toWorld[3][1] < bound_down)
        {
            numOfAsteroidsPassed++;
            reset(asteroids[i]);
        }
    }
}

// Get a random float between max and min values
float AsteroidGroup::randFloat(float max, float min)
{
    return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
}


// Give a randomized the position of the asteroids
glm::vec4 AsteroidGroup::randomPosition()
{
    float x_rand = randFloat(bound_right, bound_left);
    float y_rand = randFloat(bound_top+35,bound_top);
    float z_rand = randFloat(bound_z_pos, bound_z_neg);
    
    return glm::vec4(x_rand, y_rand, z_rand, 1.0f);
}

// Give a randomized velocity
glm::vec3 AsteroidGroup::randomVelocity()
{
    return glm::vec3(0.0f,randFloat(0.0f,-1.0f),0.0f) * speed_multiplier;
}

// Reset asteroid if out of bound
void AsteroidGroup::reset(Asteroid* asteroid)
{
    asteroid->setPosition(randomPosition());
    asteroid->velocity = randomVelocity();
}


void AsteroidGroup::update()
{
    moveAsteroids();
    checkBounds();
}




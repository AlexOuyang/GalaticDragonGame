
#include "OBJObject.h"
#include "AsteroidManager.h"

int AsteroidGroup::num_of_asteroids_passed = 0;

AsteroidGroup::AsteroidGroup(int num_of_asteroids, float bound_top, float bound_down,
                             float bound_left, float bound_right, float bound_z_pos, float bound_z_neg):
num_of_asteroids(num_of_asteroids),
bound_top(bound_top),
bound_down(bound_down),
bound_left(bound_left),
bound_right(bound_right),
bound_z_pos(bound_z_pos),
bound_z_neg(bound_z_neg)
{
    
    for(int i = 0; i < num_of_asteroids; i ++)
    {
        addAsteroids(randomize_position());
    }
//    addAsteroids(bound_left + avg_space_between_asteroid_x * (i+1), 0);

//    
//    float avg_space_between_asteroid = (bound_right - bound_left) / (float)(num_of_asteroids + 1);
//    for(int i = 0; i < num_of_asteroids; i++)
//    {
//        std::cout << "Asteroid created at x position: " << bound_left + avg_space_between_asteroid * (i+1) << std::endl;
//        addAsteroids(bound_left + avg_space_between_asteroid * (i+1), 0);
//    }
}

void AsteroidGroup::addAsteroids(glm::vec4 new_position)
{
    Asteroid * asteroid = new Asteroid("../../Models/sphere.obj");
    asteroid->material.k_a = glm::vec3(1);
    asteroid->material.k_d = glm::vec3(1);
    asteroid->material.k_s = glm::vec3(1);
    asteroid->material.shininess = 0;
    asteroid->toWorld[3] = new_position;
    asteroid->original_pos = new_position;
    asteroid->velocity = glm::vec3(0.0f,randFloat(0.0f,-1.0f),0.0f);
    asteroids.push_back(asteroid);
}

void AsteroidGroup::moveAsteroids()
{
    for(int i = 0; i < asteroids.size(); i ++){
        asteroids[i]->translate(asteroids[i]->velocity);
    }
}

void AsteroidGroup::checkBounds(){
    for(int i = 0; i < asteroids.size(); i ++){
        if(asteroids[i]->toWorld[3][1] < bound_down)
        {
//            std::cerr << "BOUND REACHED" << std::endl;
            num_of_asteroids_passed ++;
            reset(asteroids[i]);
        }
    }
}

float AsteroidGroup::randFloat(float max, float min)
{
    return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
}

glm::vec4 AsteroidGroup::randomize_position()
{
    float x_rand = randFloat(bound_right, bound_left);
    float y_rand = randFloat(bound_top+35,bound_top);
    float z_rand = randFloat(bound_z_pos, bound_z_neg);
    
    return glm::vec4(x_rand, y_rand, z_rand, 1.0f);
}

void AsteroidGroup::reset(Asteroid* asteroid)
{
    asteroid->toWorld[3] = randomize_position();
    asteroid->velocity = glm::vec3(0.0f,randFloat(0.0f,-1.0f),0.0f);
}

    
    
    

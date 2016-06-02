
#include "OBJObject.h"
#include "AsteroidManager.h"



AsteroidGroup::AsteroidGroup(int num_of_asteroids, float bound_top, float bound_down,
              float bound_left, float bound_right):
num_of_asteroids(num_of_asteroids),
bound_top(bound_top),
bound_down(bound_down),
bound_left(bound_left),
bound_right(bound_right)
{
    float avg_space_between_asteroid = (bound_right - bound_left) / (float)(num_of_asteroids + 1);
    for(int i = 0; i < num_of_asteroids; i++)
    {
        std::cout << "Asteroid created at x position: " << bound_left + avg_space_between_asteroid * (i+1) << std::endl;
        addAsteroids(bound_left + avg_space_between_asteroid * (i+1));
    }
}

void AsteroidGroup::addAsteroids(float offset)
{
    Asteroid * asteroid = new Asteroid("../../Models/sphere.obj");
    asteroid->material.k_a = glm::vec3(1);
    asteroid->material.k_d = glm::vec3(1);
    asteroid->material.k_s = glm::vec3(1);
    asteroid->material.shininess = 0;
    asteroid->translate(offset, bound_top , 0);
    asteroids.push_back(asteroid);
}

void AsteroidGroup::moveAsteroids()
{
    for(int i = 0; i < asteroids.size(); i ++){
        asteroids[i]->translate(0.0f,-0.01f,0.0f);
    }
}
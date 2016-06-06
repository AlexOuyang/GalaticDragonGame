#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


/**
 * AABB Bounding box
 * region R = (x, y, z) | |c.x-x|<=rx, |c.y-y|<=ry, |c.z-z|<=rz
 */
class BoundingBox
{
private:
    float angle;
    GLuint VBO, VAO, EBO;

public:
    glm::vec3 center; // center point of AABB
    glm::vec3 r; // radius or halfwidth extents (rx, ry, rz)
    glm::mat4 toWorld;
    
	BoundingBox();
	~BoundingBox();

	void draw(GLuint);
	void update();
};

#endif


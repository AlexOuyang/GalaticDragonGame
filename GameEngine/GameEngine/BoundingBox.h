#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "OBJObject.h"

class OBJObject;

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
    static std::vector<BoundingBox*> boundingBoxes;
    
    OBJObject * parentObj;
    glm::vec3 center; // center point of AABB
    glm::vec3 r; // radius or halfwidth extents (rx, ry, rz)
    glm::mat4 toWorld;
    
    BoundingBox(OBJObject * obj);
	~BoundingBox();

	void draw(GLuint);
	void update(glm::vec3 objectPos, glm::vec3 objectScale);

    void setRadius(glm::vec3 r);
    void setCenter(glm::vec3 pos);
};

#endif


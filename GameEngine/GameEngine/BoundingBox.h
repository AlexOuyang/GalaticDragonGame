#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/ext.hpp" // print out glm matrices
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
    
    // Offseet are used to position the bounding box away form the center
    glm::vec3 pos_offset;

public:
    static std::vector<BoundingBox*> boundingBoxes;
    bool collided;
    OBJObject * parentObj;
    glm::vec3 center; // center point of AABB
    glm::vec3 r; // radius or halfwidth extents (rx, ry, rz)
    glm::mat4 toWorld;
    
    BoundingBox(OBJObject * obj);
	~BoundingBox();

	void draw(GLuint);
	void update(glm::vec3 objectPos);

//    void setRadius(glm::vec3 r);
    void setCenter(glm::vec3 pos);
    
    // Scale the radius, or halfwidth extents
    void scale(float ratio);
    void scale(glm::vec3 ratio);
    void translate(glm::vec3 vec);

    
};

#endif


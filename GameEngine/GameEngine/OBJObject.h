#ifndef OBJOBJECT_H
#define OBJOBJECT_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string/predicate.hpp> // Check for string prefixes
#include <boost/algorithm/string/classification.hpp> // Delimiters for splitting strings
#include <boost/algorithm/string/split.hpp> // Split strings




/**
 * Object material property
 */
struct Material
{
    glm::vec3 k_a; // Ambient coefficient, it is used to define the "inherent color" of the object.
    glm::vec3 k_d; // Material Diffuse reflectance, diffuse material is a material that reflects light rays in many directions
    glm::vec3 k_s; // Specular coefficient, A Specular material is to simulate the effect of shiny objects(such as in metals) creating a "spec" where the reflection direction and eye match up.
    GLfloat shininess; // Denoted alpha. It is how much the specular is dispersed over the object. The higher this value, the smaller the highlight "spec".
};

/**
 * Object transform property
 */
struct Transform
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};


/**
 * Object is initialized to be a standard cube(a 2x2x2 cube, 
 * with all vertices in the range [-1,1])
 */
class OBJObject
{
private:
    /*==== For Rendering Obj ===*/
    GLuint VAO; // Vertex Array Object
    GLuint VBO; // Vertex Buffer Object
    GLuint NBO; // Vertex Normal Array Object
    GLuint EBO; // Element Buffer Object
    GLuint EBO2; // Element Buffer Object
    
    /*=== Obj information ====*/
    std::vector<unsigned int> vertexIndices;
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> normalIndices;
    std::vector<glm::vec3> normals;
    
    /*=== Others ===*/
    float angle;
    
    
    void parse(const char* filepath);
    void setUpVertexArrayBuffer();
    
public:
    Transform transform;
    Material material;
//    BoundingBox bound;
    glm::mat4 toWorld;
    
    OBJObject(const char* filepath);
    ~OBJObject();

    
    // Sent this object's material properties to a specified shader program
    void bindMaterialToShader(GLuint shaderProgram);
    
    void spin(float deg, glm::vec3 axis);
    void spin(float deg);
    void translate(float x, float y, float z);
    void translate(glm::vec3 vec);
    void scale(float ratio);
    void scale(glm::vec3 vec);
    void reset();
    void rotate(float rotAngle, glm::vec3 rotAxis);
    void setPosition(glm::vec3 pos);

    
    
    // This draws the model using Phong shading
    void draw(GLuint shaderProgram);
    
    // This draws the model using environmental mapping
    void drawGlossy(GLuint shaderProgram);
    
    //This draws the model using SSAO
    void drawSSAO(GLuint shaderProgram);
    
    // Collision detection from this object to other game object
    bool onCollision(OBJObject * gameObject);

};

#endif
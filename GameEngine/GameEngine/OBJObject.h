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



class CoasterTrack;


// Material settings
struct Material
{
    glm::vec3 k_a; // Ambient coefficient, it is used to define the "inherent color" of the object.
    glm::vec3 k_d; // Material Diffuse reflectance, diffuse material is a material that reflects light rays in many directions
    glm::vec3 k_s; // Specular coefficient, A Specular material is to simulate the effect of shiny objects(such as in metals) creating a "spec" where the reflection direction and eye match up.
    GLfloat shininess; // Denoted alpha. It is how much the specular is dispersed over the object. The higher this value, the smaller the highlight "spec".
};


class OBJObject
{
private:
    /*==== For Rendering Obj ===*/
    GLuint VAO; // Vertex Array Object
    GLuint VBO; // Vertex Buffer Object
    GLuint EBO; // Element Buffer Object
    GLuint NBO; // Vertex Normal Array Object

  
    /*=== Obj properties ===*/
    float angle;
    float objGravity;
    float objectSize;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    
    // Sent this object's material properties to a specified shader program
    void bindMaterialToShader(GLuint shaderProgram);
    
public:
    Material material;
    glm::mat4 toWorld;
    
    OBJObject(const char* filepath);
    ~OBJObject();
    
    void parse(const char* filepath);
    
    void update();
    void spin(float deg);
    void translate(float x, float y, float z);
    void translate(glm::vec3 vec);
    void scale(float ratio);
    void reset();
    void rotate(float rotAngle, glm::vec3 rotAxis);
    void setPosition(glm::vec3 pos);
    
    
    // This draws the model using Phong shading
    void draw(GLuint shaderProgram);
    
    // This draws the model using environmental mapping
    void drawGlossy(GLuint shaderProgram);
    
    //This draws the model using SSAO
    void drawSSAO(GLuint shaderProgram);
};

#endif
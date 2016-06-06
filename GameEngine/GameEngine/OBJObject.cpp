
#include "glm/ext.hpp" // print out glm matrices
#include "Window.h"
#include "Light.h"
#include "SSAO.h"
#include "BoundingBox.h"

#include "OBJObject.h"



OBJObject::OBJObject(const char *filepath)
{
    this->tag = "default";
    
    // Default transform
    this->transform.position = glm::vec3(0);
    this->transform.scale = glm::vec3(1.0f);
    
    // Default materials are zero vectors
    this->material.k_a = glm::vec3(0.19225f, 0.19225f, 0.19225f);
    this->material.k_d = glm::vec3(0.50754f, 0.50754f, 0.50754f);
    this->material.k_s = glm::vec3(0.508273f, 0.508273f, 0.508273f);
    this->material.shininess = 0.4f * Light::LIGHT_SHINENESS_COEFFICIENT;
    //    this->material.k_a = glm::vec3(1.0f);
    //    this->material.k_s = glm::vec3(1.0f);
    //    this->material.k_d = glm::vec3(1.0f);
    //    this->material.shininess = 1.0f;
    
    
    this->toWorld = glm::mat4(1.0f);
    this->angle = 0.0f;
    
    parse(filepath);
    
    setUpVertexArrayBuffer();
}




void OBJObject::setUpVertexArrayBuffer()
{
    // Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // Create a variable called VBO to hold all the vertices vector in the GPU
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &NBO);
    
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
    
    /*====== Sent vertices information from CPU to GPU, order matters =======*/
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    
    // This gives vertex position and offset information to the shader
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    glEnableVertexAttribArray(0);
    
    
    /*====== Sent vertexIndices information from CPU to GPU =======*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vertexIndices.size(), &vertexIndices[0], GL_STATIC_DRAW);
    
    
    /*====== Sent vertices information from CPU to GPU =======*/
    // Sent information of normals from CPU to GPU
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * normals.size(), &normals[0], GL_STATIC_DRAW);
    
    //    // This gives vertex position and offset information to the shader
    //    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
    //                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
    //                          GL_FLOAT, // What type these components are
    //                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
    //                          3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
    //                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    //    glEnableVertexAttribArray(0);
    //
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    //    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * normalIndices.size(), &normalIndices[0], GL_STATIC_DRAW);
    //
    
    /*====== Sent vertex array information to the shader =======*/
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}



OBJObject::~OBJObject() {
    if (boundingBox != nullptr) delete boundingBox;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &NBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &EBO2);
}



void OBJObject::parse(const char *filepath)
{
    float minX = FLT_MAX; // Set min to the largest number possible then update it
    float minY = FLT_MAX;
    float minZ = FLT_MAX;
    float maxX = FLT_MIN; // Set min to the largest number possible then update it
    float maxY = FLT_MIN;
    float maxZ = FLT_MIN;
    
    //TODO parse the OBJ file
    // Populate the face vertexIndices, vertices, and normals vectors with the OBJ Object data
    std::ifstream infile(filepath, std::ifstream::in);
    std::string line;
    
    if (infile.is_open())
    {
        while (std::getline(infile, line))
        {
            if (boost::starts_with(line, "vn"))
            {
                // Split the strings by space
                std::vector<std::string> strs;
                boost::split(strs, line, boost::is_any_of(" "), boost::token_compress_on);
                
                // Create a normal vector3
                glm::vec3 n;
                n.x = std::stof (strs[1]);
                n.y = std::stof (strs[2]);
                n.z = std::stof (strs[3]);
                normals.push_back(n);
                
                //                                std::cout << "vn: " << n.x << " " << n.y << " " << n.z << "\n";
                
            }
            else if (boost::starts_with(line, "vt")) // We ignore texture mapping
            {
                continue;
            }
            else if (boost::starts_with(line, "v"))
            {
                // Split the strings by space
                std::vector<std::string> strs;
                boost::split(strs, line, boost::is_any_of(" "), boost::token_compress_on);
                
                // Create a vertex vector3
                glm::vec3 vtx;
                vtx.x = std::stof (strs[1]);
                vtx.y = std::stof (strs[2]);
                vtx.z = std::stof (strs[3]);
                vertices.push_back(vtx);
                
                
                // Update min and max dimentions of the model
                if (vtx.x < minX) minX = vtx.x;
                if (vtx.y < minY) minY = vtx.y;
                if (vtx.z < minZ) minZ = vtx.z;
                
                if (vtx.x > maxX) maxX = vtx.x;
                if (vtx.y > maxY) maxY = vtx.y;
                if (vtx.z > maxZ) maxZ = vtx.z;
                
                //                                std::cout << "v: " << vtx.x << " " << vtx.y << " " << vtx.z << "\n";
            }
            else if (boost::starts_with(line, "f"))
            {
                // Split the strings by space
                std::vector<std::string> strs;
                // Split by ' ' or /
                boost::split(strs, line, boost::is_any_of(" /"), boost::token_compress_on);
                
                // std::cout << strs.size() << std::endl;
                
                
                // If the object's doesn't have texture mapping
                // ex: 1//1, then there are only 7 elements
                if (strs.size() == 7)
                {
                    // Push vertex indice into vertexIndices
                    // Since the face starts from 0th index in the vertices vector, so we need to offset the face index by 1
                    vertexIndices.push_back(std::stoi (strs[1])-1);
                    vertexIndices.push_back(std::stoi (strs[3])-1);
                    vertexIndices.push_back(std::stoi (strs[5])-1);
                    
                    normalIndices.push_back(std::stoi (strs[2])-1);
                    normalIndices.push_back(std::stoi (strs[4])-1);
                    normalIndices.push_back(std::stoi (strs[6])-1);
                }
                else if (strs.size() == 10) // There are 10 elements, ex: 1/2/3
                {
                    vertexIndices.push_back(std::stoi (strs[1])-1);
                    vertexIndices.push_back(std::stoi (strs[4])-1);
                    vertexIndices.push_back(std::stoi (strs[7])-1);
                    
                    normalIndices.push_back(std::stoi (strs[3])-1);
                    normalIndices.push_back(std::stoi (strs[6])-1);
                    normalIndices.push_back(std::stoi (strs[9])-1);
                }
                else
                {
                    std::cerr << filepath << " has weird face vertexIndices" << std::endl;
                    exit(-1);
                }
                
                //                                std::cout << "f: " << std::stoi (strs[1]) << " " << std::stoi (strs[3]) <<" " << std::stoi (strs[5]) << "\n";
                
            }
        }
        infile.close();
        
        // Check for empty file
        if (vertexIndices.size() == 0) {
            std::cerr << "Model has not vertices" << std::endl;
            exit(1);
        }
        //        std::cout << "indice size" << vertexIndices.size() << std::endl;
        
        // Center the model by subtractng the model from the average
        // Also, normalize the scale of the model into a standard cube(a 2x2x2 cube,
        // with all vertices in the range [-1,1])
        float num_of_vertices =vertices.size();
        float avgX = (maxX + minX) / 2;
        float avgY = (maxY + minY) / 2;
        float avgZ = (maxZ + minZ) / 2;
        std::vector<float> dims;
        dims.push_back(maxX - minX);
        dims.push_back(maxY - minY);
        dims.push_back(maxZ - minZ);
        float longest_axis = * std::max_element(dims.begin(), dims.end());
        for (int i = 0; i < num_of_vertices; i++) {
            vertices[i].x = (vertices[i].x - avgX) / longest_axis;
            vertices[i].y = (vertices[i].y - avgY) / longest_axis;
            vertices[i].z = (vertices[i].z - avgZ) / longest_axis;
        }
        
    }
    else std::cout << "Invalid file can not be opened";
}

void OBJObject::draw(GLuint shaderProgram)
{
    glUseProgram(shaderProgram);
    
    bindMaterialToShader(shaderProgram);
    
    // Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
    //    glm::mat4 MVP = Window::P * Window::V * toWorld;
    // We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009), gl_ModelViewProjectionMatrix has been
    // removed from the language. The user is expected to supply this matrix to the shader when using modern OpenGL.
    //    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    //    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    
    GLuint P_mat = glGetUniformLocation(shaderProgram, "perspective");
    glUniformMatrix4fv(P_mat, 1, GL_FALSE, &Window::P[0][0]);
    
    GLuint C_inverse_mat = glGetUniformLocation(shaderProgram, "camera");
    glUniformMatrix4fv(C_inverse_mat, 1, GL_FALSE, &Window::V[0][0]);
    
    GLuint M_mat = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(M_mat, 1, GL_FALSE, &toWorld[0][0]);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (int)vertexIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void OBJObject::drawGlossy(GLuint shaderProgram)
{
    glUseProgram(shaderProgram);
    
    GLuint P_mat = glGetUniformLocation(shaderProgram, "perspective");
    glUniformMatrix4fv(P_mat, 1, GL_FALSE, &Window::P[0][0]);
    
    GLuint C_inverse_mat = glGetUniformLocation(shaderProgram, "camera");
    glUniformMatrix4fv(C_inverse_mat, 1, GL_FALSE, &Window::V[0][0]);
    
    GLuint M_mat = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(M_mat, 1, GL_FALSE, &toWorld[0][0]);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (int)vertexIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



// Sent this object's material properties to a specified shader program
void OBJObject::bindMaterialToShader(GLuint shaderProgram)
{
    GLuint materialAttrID = glGetUniformLocation(shaderProgram, "material.k_a");
    glUniform3fv(materialAttrID, 1, &this->material.k_a[0]);
    materialAttrID = glGetUniformLocation(shaderProgram, "material.k_d");
    glUniform3fv(materialAttrID, 1, &this->material.k_d[0]);
    materialAttrID = glGetUniformLocation(shaderProgram, "material.k_s");
    glUniform3fv(materialAttrID, 1, &this->material.k_s[0]);
    materialAttrID = glGetUniformLocation(shaderProgram, "material.shininess");
    glUniform1f(materialAttrID, this->material.shininess);
}




void OBJObject::reset()
{
    toWorld = glm::mat4(1.0f);
    angle = 0.0f;
}



void OBJObject::spin(float deg, glm::vec3 axis)
{
    this->angle += deg;
    if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
    auto rotationMat = glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), axis);
    this->toWorld = this->toWorld * rotationMat;
}

// Spin based on transform rotation
void OBJObject::spin(float deg)
{
    spin(deg, this->transform.rotation);
}


void OBJObject::translate(float x, float y, float z)
{
    auto translationMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    this->toWorld = translationMat * this->toWorld;
}

void OBJObject::translate(glm::vec3 vec)
{
    auto translationMat = glm::translate(glm::mat4(1.0f), vec);
    this->toWorld = translationMat * this->toWorld;
}

void OBJObject::scale(float ratio)
{
    auto scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(ratio));
    this->toWorld = this->toWorld * scaleMat;
}

void OBJObject::scale(glm::vec3 vec)
{
    auto scaleMat = glm::scale(glm::mat4(1.0f), vec);
    this->toWorld = this->toWorld * scaleMat;
}


void OBJObject::rotate(float rotAngle, glm::vec3 rotAxis)
{
    auto rotationMat = glm::rotate(glm::mat4(1.0f), rotAngle / 180.0f * glm::pi<float>(), rotAxis);
    this->toWorld = rotationMat * this->toWorld;
}


void OBJObject::setPosition(glm::vec3 pos)
{
    this->toWorld[3] = glm::vec4(pos, 1);
}




/*=========== SSAO ============*/

void OBJObject::drawSSAO(GLuint shaderProgram)
{
    GLuint M_mat = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(M_mat, 1, GL_FALSE, &toWorld[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &Window::V[0][0]);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (int)vertexIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


/*========= Collision Detection ===========*/


void OBJObject::createBoundingBox()
{
    // By Default, the bounding box has size rx = 0.5, ry = 0.5, rz = 0.5
    this->boundingBox = new BoundingBox(this);
}

BoundingBox * OBJObject::getBoundingBox()
{
    return (boundingBox != nullptr) ? boundingBox : nullptr;
}

void OBJObject::updateBoundingBox()
{
    if (boundingBox != nullptr)
        boundingBox->update(glm::vec3(toWorld[3]), glm::vec3(toWorld[0][0], toWorld[1][1], toWorld[2][2]));
}

bool OBJObject::onCollision()
{
    BoundingBox * a = boundingBox;
    for (int i = 0; i < BoundingBox::boundingBoxes.size(); i++)
    {
        BoundingBox * b = BoundingBox::boundingBoxes[i];
        if (b != a)
        {
            std::cout << i << "    " << b->parentObj->tag << "    " << glm::to_string(b->r) << std::endl;
            int r;
            r = a->r[0] + b->r[0]; if ((unsigned int)(a->center[0] - b->center[0] + r) > r + r) return false;
            r = a->r[1] + b->r[1]; if ((unsigned int)(a->center[1] - b->center[1] + r) > r + r) return false;
            r = a->r[2] + b->r[2]; if ((unsigned int)(a->center[2] - b->center[2] + r) > r + r) return false;
        }
    }
    // Overlapping on all axes means AABBs are intersecting
    return true;
}


void OBJObject::drawBoundingBox(GLint shaderProgram)
{
    glUseProgram(shaderProgram);
    updateBoundingBox();
    boundingBox->draw(shaderProgram);
}



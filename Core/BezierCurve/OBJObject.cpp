
#include "glm/ext.hpp" // print out glm matrices
#include "Window.h"
#include "BezierCurve.h"
#include "CoasterTrack.h"

#include "OBJObject.h"


OBJObject::OBJObject(const char *filepath)
{
    // Default materials are zero vectors
    material.k_a = glm::vec3(0.0f);
    material.k_s = glm::vec3(0.0f);
    material.k_d = glm::vec3(0.0f);
    material.shininess = 0.0f;
    
    toWorld = glm::mat4(1.0f);
    angle = 0.0f;
    objGravity = 0.5f; // Gravity coefficient
    objectSize = 1.0f;
    
    parse(filepath);
    
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
    
    
    /*====== Sent indices information from CPU to GPU =======*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
    
    
    /*====== Sent vertices information from CPU to GPU =======*/
    // Sent information of normals from CPU to GPU
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * normals.size(), &normals[0], GL_STATIC_DRAW);
    
    // This gives vertex information to the shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    
    
    
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

OBJObject::~OBJObject() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &NBO);
    
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
    // Populate the face indices, vertices, and normals vectors with the OBJ Object data
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
            else if (boost::starts_with(line, "vt"))
            {
                
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
            }
            else if (boost::starts_with(line, "f"))
            {
                // Split the strings by space
                std::vector<std::string> strs;
                // Split by ' ' or /
                boost::split(strs, line, boost::is_any_of(" /"), boost::token_compress_on);
                // Push vertex indice into indices
                // Since the face starts from 0th index in the vertices vector, so we need to offset the face index by 1
//                std::cout << line << std::endl;
                indices.push_back(std::stoi (strs[1])-1);
                indices.push_back(std::stoi (strs[4])-1);
                indices.push_back(std::stoi (strs[7])-1);
                
                indices.push_back(std::stoi (strs[1])-1);
                indices.push_back(std::stoi (strs[4])-1);
                indices.push_back(std::stoi (strs[7])-1);
                
//                std::cout << "f: " << std::stoi (strs[1]) << " " << std::stoi (strs[2]) << " " << std::stoi (strs[3])<< " " << std::stoi (strs[4])<< " " << std::stoi (strs[5])<< " " << std::stoi (strs[6])<< " " << std::stoi (strs[7])<< " " << std::stoi (strs[8])<< " " << std::stoi (strs[9]) << "\n";
                
            }
        }
        infile.close();
        
        // Check for empty file
        if (indices.size() == 0) {
            std::cerr << "Model has not vertices" << std::endl;
            exit(1);
        }
        
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
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void OBJObject::drawGlossy(GLuint shaderProgram)
{
    
    GLuint P_mat = glGetUniformLocation(shaderProgram, "perspective");
    glUniformMatrix4fv(P_mat, 1, GL_FALSE, &Window::P[0][0]);
    
    GLuint C_inverse_mat = glGetUniformLocation(shaderProgram, "camera");
    glUniformMatrix4fv(C_inverse_mat, 1, GL_FALSE, &Window::V[0][0]);
    
    GLuint M_mat = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(M_mat, 1, GL_FALSE, &toWorld[0][0]);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


// Called in draw function before drawing things
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


void OBJObject::update()
{
    spin(1.0f);
}

void OBJObject::spin(float deg)
{
    this->angle += deg;
    if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
    auto rotationMat = glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    this->toWorld = this->toWorld * rotationMat;
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
    objectSize = ratio;
    auto scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(ratio));
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


/*============= Rollercoaster ============*/

// Called moveInTrack to get the velocity based on current and next height
float OBJObject::calculateVelocity(float max_height, float cur_height)
{
    return (float)sqrt(2 * objGravity * (max_height-cur_height));
}

float current_accumulated_time_step = 0.0f; // This determins where in the curve the train starts at
float total_num_of_steps = 100.0f;
// Move the object in bezier curve track
void OBJObject::moveInTrack(CoasterTrack * rollerCosterTrack)
{
    glm::vec4 next_pos;
    glm::vec4 cur_pos;
    
    // Switch track to the next track
    if (current_accumulated_time_step > total_num_of_steps)
    {
        rollerCosterTrack->setNextBezierCurve();
        current_accumulated_time_step = 0;
    }
    
    cur_pos = rollerCosterTrack->getCurrentBezierCurve()->getIntepolatedPosInCurve(current_accumulated_time_step);
    float v = calculateVelocity(rollerCosterTrack->maxHeightPos.y, cur_pos.y);
    if (isnan(v)) return;

//    std::cout << "highest y: " << rollerCosterTrack->maxHeightPos.y << std::endl;
//    std::cout << "current y: " << cur_pos.y << std::endl;
    
    
    // give the roller coaster an initial push at first
    if (v <= 0.1f) current_accumulated_time_step += 5;
    else current_accumulated_time_step += v;
    
    next_pos = rollerCosterTrack->getCurrentBezierCurve()->getIntepolatedPosInCurve(current_accumulated_time_step);
    

    std::cout << "current velocity: " << v << std::endl;
//    std::cout << "accumulated time step: " << current_accumulated_time_step << std::endl;
    
    
    // If the next position is out of bound/doesn't exist, do nothing
    if (isnan(next_pos.x)) return;
    
    // Change orientation and position
    updateOrientation(cur_pos, next_pos);
    this->toWorld[3] = cur_pos;
    
}


// Update the object's orientation to face the next position its traveling to
void OBJObject::updateOrientation(glm::vec3 curPos, glm::vec3 nextPos)
{
    glm::vec3 up_temp = glm::vec3(0, 1, 0);
    
    glm::vec3 z = glm::normalize(nextPos - curPos);
    glm::vec3 x = glm::normalize(glm::cross(up_temp, z));
    glm::vec3 y = glm::cross(z, x);
    
    toWorld[0] = glm::vec4(x, 0);
    toWorld[1] = glm::vec4(y, 0);
    toWorld[2] = glm::vec4(z, 0);
    scale(objectSize);
}

void OBJObject::setCurrentAccumulatedTimeStep(float t)
{
    current_accumulated_time_step = t * 100;
    std::cout << "accumulated time step: " << t << std::endl;

}




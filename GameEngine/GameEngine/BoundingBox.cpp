#include "Window.h"

#include "BoundingBox.h"


std::vector<BoundingBox*> BoundingBox::boundingBoxes;


GLfloat boundingBoxVertices[] = {
	// Front vertices
	-0.5, -0.5,  0.5,
	0.5, -0.5,  0.5,
	0.5,  0.5,  0.5,
	-0.5,  0.5,  0.5,
	// Back vertices
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5,  0.5, -0.5,
	-0.5,  0.5, -0.5
};

//GLfloat boundingBoxVertices[] = {
//    // Front vertices
//    -1, -1,  1,
//    1, -1,  1,
//    1,  1,  1,
//    -1,  1,  1,
//    // Back vertices
//    -1, -1, -1,
//    1, -1, -1,
//    1,  1, -1,
//    -1,  1, -1
//};

GLuint boundingBoxIndices[] = {  // Note that we start from 0!
	// Front face
	0, 1, 2,
	2, 3, 0,
	// Top face
	1, 5, 6,
	6, 2, 1,
	// Back face
	7, 6, 5,
	5, 4, 7,
	// Bottom face
	4, 0, 3,
	3, 7, 4,
	// Left face
	4, 5, 1,
	1, 0, 4,
	// Right face
	3, 2, 6,
	6, 7, 3
};

BoundingBox::BoundingBox(OBJObject * obj)
{
    boundingBoxes.push_back(this);
    
    this->pos_offset = glm::vec3(0);
    this->collided = false;
    this->parentObj = obj;
    this->toWorld = glm::mat4(1.0f);
    this->angle = 0.0f;
    this->center = glm::vec3(0);
    this->r = glm::vec3(0.5f);

	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boundingBoxVertices), boundingBoxVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boundingBoxIndices), boundingBoxIndices, GL_STATIC_DRAW);

	
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

BoundingBox::~BoundingBox()
{
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}



void BoundingBox::setCenter(glm::vec3 pos)
{
    this->center = pos;
    this->toWorld[3] = glm::vec4(this->center + this->pos_offset, 1);
}

void BoundingBox::scale(float ratio)
{
    this->r = this->r * ratio;
    auto scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(ratio));
    this->toWorld = this->toWorld * scaleMat;
}

void BoundingBox::scale(glm::vec3 ratio)
{
    this->r = this->r * ratio;
    auto scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(ratio));
    this->toWorld = this->toWorld * scaleMat;
}


void BoundingBox::translate(glm::vec3 vec)
{
    this->pos_offset = vec;
}

void BoundingBox::draw(GLuint shaderProgram)
{
    glUseProgram(shaderProgram);
    
    glUniform1i(glGetUniformLocation(shaderProgram, "collided"), this->collided);
    
    GLuint P_mat = glGetUniformLocation(shaderProgram, "perspective");
    glUniformMatrix4fv(P_mat, 1, GL_FALSE, &Window::P[0][0]);
    
    GLuint C_inverse_mat = glGetUniformLocation(shaderProgram, "camera");
    glUniformMatrix4fv(C_inverse_mat, 1, GL_FALSE, &Window::V[0][0]);
    
    GLuint M_mat = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(M_mat, 1, GL_FALSE, &toWorld[0][0]);

    glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void BoundingBox::update(glm::vec3 objectPos)
{
    this->setCenter(objectPos);
}






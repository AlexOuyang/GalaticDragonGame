#include "Window.h"
#include "BoundingBox.h"

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

BoundingBox::BoundingBox()
{
    
	this->toWorld = glm::mat4(1.0f);

	this->angle = 0.0f;

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

void BoundingBox::draw(GLuint shaderProgram)
{
    glUseProgram(shaderProgram);
    
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

void BoundingBox::update()
{
}


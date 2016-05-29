//
//  BezierCurve.cpp
//  BezierCurve
//
//  Created by Chenxing Ouyang on 5/12/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#include "Window.h"
#include "BezierCurve.h"


BezierCurve::BezierCurve(ControlObj * c0, ControlObj * c1,ControlObj * c2, ControlObj * c3,
                         float lineWidth, float controlPointSize):
c0(c0),
c1(c1),
c2(c2),
c3(c3)
{
    this->toWorld = glm::mat4(1.0f);
    
    this->lineWidth = lineWidth;
    
    this->controlPointSize = controlPointSize;
    
    this->numOfPoints = 100.0f; // 100 points by default
    
    this->curveColor = glm::vec4(1.0f, 0.024, 0, 1.0f);
    this->controlPointColor = glm::vec4(0.0f, 0.2f, 1.0f, 1.0f);
    this->anchorPointColor = glm::vec4(1.0f, 0.494f, 0.302f, 1.0f);
    
    for (int i = 0; i < 4; i++)
        C3_i.push_back(binomialCoeff(3, i));
    
    // Get curve points and put them into VBO
    std::vector<glm::vec4> curvePoints = calculateCurve();
    // Push control points to draw as well
    curvePoints.push_back(c0->pos);
    curvePoints.push_back(c1->pos);
    curvePoints.push_back(c2->pos);
    curvePoints.push_back(c3->pos);
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * curvePoints.size(), &curvePoints[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
                          4 * sizeof(GLfloat) // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          , (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
};


BezierCurve::~BezierCurve() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

// This draws all control points in the selection buffer
void BezierCurve::draw(GLuint shaderProgram)
{
    GLuint P_mat = glGetUniformLocation(shaderProgram, "perspective");
    glUniformMatrix4fv(P_mat, 1, GL_FALSE, &Window::P[0][0]);
    
    GLuint C_inverse_mat = glGetUniformLocation(shaderProgram, "camera");
    glUniformMatrix4fv(C_inverse_mat, 1, GL_FALSE, &Window::V[0][0]);
    
    GLuint M_mat = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(M_mat, 1, GL_FALSE, &toWorld[0][0]);
    
    glBindVertexArray(VAO);
    // Draw the curve
    GLuint color = glGetUniformLocation(shaderProgram, "color");
    glUniform4fv(color, 1, &curveColor[0]);
    glLineWidth(lineWidth);
    for(int i = 0; i < 99; i++) glDrawArrays(GL_LINES, i, 2);
    // Draw handle lines between control points
    color = glGetUniformLocation(shaderProgram, "color");
    glUniform4fv(color, 1, &controlPointColor[0]);
    glDrawArrays(GL_LINES, 100, 2);
    glDrawArrays(GL_LINES, 102, 2);
    // Draw 4 control points
    glPointSize(controlPointSize);
    color = glGetUniformLocation(shaderProgram, "color");
    glUniform4fv(color, 1, &anchorPointColor[0]);
    glDrawArrays(GL_POINTS, 100, 1);
    glDrawArrays(GL_POINTS, 103, 1);
    color = glGetUniformLocation(shaderProgram, "color");
    glUniform4fv(color, 1, &controlPointColor[0]);
    glDrawArrays(GL_POINTS, 101, 1);
    glDrawArrays(GL_POINTS, 102, 1);
    glBindVertexArray(0);
}


void BezierCurve::drawSelectionBuffer(GLuint shaderProgram)
{
    glUseProgram(shaderProgram);
    GLuint P_mat = glGetUniformLocation(shaderProgram, "perspective");
    glUniformMatrix4fv(P_mat, 1, GL_FALSE, &Window::P[0][0]);
    
    GLuint C_inverse_mat = glGetUniformLocation(shaderProgram, "camera");
    glUniformMatrix4fv(C_inverse_mat, 1, GL_FALSE, &Window::V[0][0]);
    
    GLuint M_mat = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(M_mat, 1, GL_FALSE, &toWorld[0][0]);
    
    glBindVertexArray(VAO);
    // Draw 4 control points basd on id
    glPointSize(controlPointSize);
    
    GLuint id_location = glGetUniformLocation(shaderProgram, "id");
    glUniform1ui(id_location, c0->ID);
    glDrawArrays(GL_POINTS, 100, 1);
    
    id_location = glGetUniformLocation(shaderProgram, "id");
    glUniform1ui(id_location, c1->ID);
    glDrawArrays(GL_POINTS, 101, 1);

    id_location = glGetUniformLocation(shaderProgram, "id");
    glUniform1ui(id_location, c2->ID);
    glDrawArrays(GL_POINTS, 102, 1);

    id_location = glGetUniformLocation(shaderProgram, "id");
    glUniform1ui(id_location, c3->ID);
    glDrawArrays(GL_POINTS, 103, 1);
    
    glBindVertexArray(0);
}

// Returns 100 inteporlated points that forms the bezier curve
std::vector<glm::vec4> BezierCurve::calculateCurve()
{
    std::vector<glm::vec4> curve; // w = 1 for position
    
    float t = 1.0f/numOfPoints;
    
    glm::mat4 P = glm::mat4(0);
    P[0] = c0->pos;
    P[1] = c1->pos;
    P[2] = c2->pos;
    P[3] = c3->pos;
    
    // Reset max height position
    maxHeightPos = glm::vec3(0, std::numeric_limits<float>::min(), 0);
    
    for (int q_index = 0; q_index < numOfPoints; q_index++)
    {
        auto current_pos = P * C(q_index * t);
        if (current_pos.y > maxHeightPos.y)
        {
            maxHeightAccumulatedTimeStep = q_index * t;
            maxHeightPos = current_pos;
        }
        curve.push_back(current_pos);
    }
    
    return curve;
}


// Updates the curve points in the VBO, only call when the control points are dragged
void BezierCurve::updateCurve()
{

    // Get curve points and put them into VBO
    std::vector<glm::vec4> curvePoints = calculateCurve();
    // Push control points to draw as well
    curvePoints.push_back(c0->pos);
    curvePoints.push_back(c1->pos);
    curvePoints.push_back(c2->pos);
    curvePoints.push_back(c3->pos);
    
    // Rebind curve points to VBO, sent information of curve points from CPU to GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * curvePoints.size(), &curvePoints[0], GL_STATIC_DRAW);
}



// This gets the current position in curve
glm::vec4 BezierCurve::getIntepolatedPosInCurve(float current_accumulated_time_step)
{
    glm::mat4 P = glm::mat4(0);
    P[0] = c0->pos;
    P[1] = c1->pos;
    P[2] = c2->pos;
    P[3] = c3->pos;
    
    return P * C(current_accumulated_time_step / numOfPoints);

}



glm::vec4 BezierCurve::C(float t)
{
    return glm::vec4(C3_i[0] * pow(t, 0) * pow(1-t, 3),
                     C3_i[1] * pow(t, 1) * pow(1-t, 2),
                     C3_i[2] * pow(t, 2) * pow(1-t, 1),
                     C3_i[3] * pow(t, 3) * pow(1-t, 0));
}


// Returns value of Binomial Coefficient C(n, k)
int BezierCurve::binomialCoeff(int n, int k)
{
    int C[n+1][k+1];
    int i, j;
    
    // Caculate value of Binomial Coefficient in bottom up manner
    for (i = 0; i <= n; i++)
    {
        for (j = 0; j <= std::min(i, k); j++)
        {
            // Base Cases
            if (j == 0 || j == i)
                C[i][j] = 1;
            
            // Calculate value using previosly stored values
            else
                C[i][j] = C[i-1][j-1] + C[i-1][j];
        }
    }
    
    return C[n][k];
}
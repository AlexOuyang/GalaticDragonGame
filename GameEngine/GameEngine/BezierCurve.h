//
//  BezierCurve.h
//  BezierCurve
//
//  Created by Chenxing Ouyang on 5/12/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#ifndef BezierCurve_h
#define BezierCurve_h


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "glm/ext.hpp" // print out glm matrices
#include "Control.h"

using namespace Control;

// Delegates to 4 control points
class BezierCurve
{
private:
    // Define control 4 control points, p0,p3 interpolating, p1,p2 used to approximating the curve
    Control::ControlObj * c0;
    Control::ControlObj * c1;
    Control::ControlObj * c2;
    Control::ControlObj * c3;
    // Curve contains 100 points calculated with t = 1/100
    std::vector<glm::vec4> curve;
    
    // Holds values of 3 choose 0, 3 choose 1, 3 choose 2, 3 choose 3
    std::vector<float> C3_i;
    
    // Returns the coefficient matrix
    glm::vec4 C(float t);
    
    // Returns value of Binomial Coefficient C(n, k)
    int binomialCoeff(int n, int k);
    
    // Total number of points interpolated in the curve
    float numOfPoints;
    
    // Returns 100 inteporlated points that forms the bezier curve
    std::vector<glm::vec4> calculateCurve();

    
public:
    glm::mat4 toWorld;
    GLuint VBO, VAO;
    float lineWidth;
    float controlPointSize;
    glm::vec4 curveColor;
    glm::vec4 controlPointColor;
    glm::vec4 anchorPointColor;
    float maxHeightAccumulatedTimeStep;
    glm::vec3 maxHeightPos;
    
    BezierCurve(Control::ControlObj * c0, ControlObj * c1,ControlObj * c2, ControlObj * c3,
                float lineWidth = 20.0f, float controlPointSize = 20.0f);
    ~BezierCurve();
    
    // This draws the interpolated bezeir curve and control handles
    void draw(GLuint shaderProgram);
    
    // Updates the curve points in the VBO, only call when the control points are dragged
    void updateCurve();
    
    // This draws all control points in the selection buffer
    void drawSelectionBuffer(GLuint shaderProgram);
    
    // This gets the current position in curve
    glm::vec4 getIntepolatedPosInCurve(float current_accumulated_time_step);
    
};


#endif /* BezierCurve_h */

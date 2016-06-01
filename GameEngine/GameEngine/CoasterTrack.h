//
//  CoasterTrack.hpp
//  BezierCurve
//
//  Created by Chenxing Ouyang on 5/18/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#ifndef CoasterTrack_h
#define CoasterTrack_h

#include <stdio.h>
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


class CoasterTrack
{
private:
    // Redefine modulus to handle negative numbers
    int mod (int a, int b);
    // Used by getAffectedControlPoints() to get circular ID
    unsigned int getIdInCircularArray(unsigned int pointID, unsigned int totalNumOfPoints);

public:
    Control::ControlManager * controlManager;
    int current_bezier_curve_in_use;
    std::vector<BezierCurve*> track;
    float maxHeightAccumulatedTimeStep;
    glm::vec3 maxHeightPos;
    
    CoasterTrack(Control::ControlManager * controlManager);
    ~CoasterTrack();

    // They are used to create and add segments that constsruct the track
    void createSegment(ControlObj * c0, ControlObj * c1, ControlObj * c2, ControlObj * c3);
    
    // Draw all bezier curves in tracks
    void draw(GLuint shaderProgram);
    
    // Draw all control points' selection buffer in bezier curves in tracks
    void drawSelectionBuffer(GLuint selectionBufferShaderProgram);
    
    // Update all curves in bezier curves
    void updateCurve();
    
    // Get the current bezier curve in use
    BezierCurve * getCurrentBezierCurve();
    
    // set the next bezier curve to use
    void setNextBezierCurve();

    // Takes in a control point's id and returns all 3 control and anchor points if the input is of
    // type ANCHOR_POINT, else return the mirror control point if input is CONTROL_POINT
    std::vector<Control::ControlObj*> getAffectedControlPoints(ControlObj * controlPoint);

    // Update the max position in terms of y and its time step, as well bezier curve in use
    void updateMaxPositionAndTimeStep();
    
};


#endif /* CoasterTrack_h */

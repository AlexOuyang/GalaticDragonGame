//
//  CoasterTrack.cpp
//  BezierCurve
//
//  Created by Chenxing Ouyang on 5/18/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#include "Control.h"
#include "BezierCurve.h"
#include "CoasterTrack.h"


CoasterTrack::CoasterTrack(Control::ControlManager * controlManager):
controlManager(controlManager),
current_bezier_curve_in_use(0)
{
    updateMaxPositionAndTimeStep();
}

CoasterTrack::~CoasterTrack()
{
    for(int i = 0; i < track.size(); i++) delete track[i];
}

void CoasterTrack::createSegment(ControlObj * c0, ControlObj * c1, ControlObj * c2, ControlObj * c3)
{
    track.push_back(new BezierCurve(c0, c1, c2, c3));
    updateMaxPositionAndTimeStep();
}

// Draw all bezier curves in tracks
void CoasterTrack::draw(GLuint shaderProgram)
{
    for(int i = 0; i < track.size(); i++) track[i]->draw(shaderProgram);
}

// Draw all control points' selection buffer in bezier curves in tracks
void CoasterTrack::drawSelectionBuffer(GLuint selectionBufferShaderProgram)
{
    for(int i = 0; i < track.size(); i++) track[i]->drawSelectionBuffer(selectionBufferShaderProgram);
}


// Update all curves in bezier curves
void CoasterTrack::updateCurve()
{
    for(int i = 0; i < track.size(); i++) track[i]->updateCurve();
}

// Get the current bezier curve in use
BezierCurve * CoasterTrack::getCurrentBezierCurve()
{
    return track[current_bezier_curve_in_use];
}

// Get the next bezier curve to use
void CoasterTrack::setNextBezierCurve()
{
    current_bezier_curve_in_use = (++current_bezier_curve_in_use) % track.size();
}


// Used by getCircularID() to handle negative modulus
int CoasterTrack::mod (int a, int b)
{
    int ret = a % b;
    if(ret < 0)
        ret+=b;
    return ret;
}

// Used by getAffectedControlPoints() to get circular ID
unsigned int CoasterTrack::getIdInCircularArray(unsigned int pointID, unsigned int totalNumOfPoints)
{
    pointID = mod(pointID,  totalNumOfPoints);
    return (pointID == 0) ? totalNumOfPoints : pointID;
}


// Takes in a control point's id and returns all 3 control and anchor points if the input is of
// type ANCHOR_POINT, else return the mirror control point if input is CONTROL_POINT
std::vector<ControlObj*> CoasterTrack::getAffectedControlPoints(ControlObj * controlPoint)
{
    unsigned int ID = controlPoint->ID;
    std::vector<ControlObj*> affected_points;
    
    // anchor points ID are: 1 4 7 10 13 16 19 22 ...
    // p2 ID are: 2 5 8 11 14 17 20 23 ...
    // p3 ID are: 3 6 9 12 15 18 21 24 ...
    if (ID <= track.size() * 3 && ID > 0)  // there can only be that many points in total
    {
        unsigned int total_num_of_control_points = controlManager->getNumOfControlPoints();
        
        if (ID % 3 == 1) // This is the anchor point, which affects the control points next to it
        {
            // If ID is 24 then modulus returns 24 % 24 = 0 which is an edge case
            auto prev_ID = getIdInCircularArray(ID - 1,  total_num_of_control_points);
            affected_points.push_back(controlManager->getControlPointByID(prev_ID));
            
            affected_points.push_back(controlManager->getControlPointByID(ID));
            
            auto next_ID = getIdInCircularArray(ID + 1,  total_num_of_control_points);
            affected_points.push_back(controlManager->getControlPointByID(next_ID));
        }
        // If anchor points are cliced, return ush all points including the clicked anchor points,
        // else only the mirror point
        else if (ID % 3 == 2) // This is ID for p2
        {
            auto prev_ID = getIdInCircularArray(ID - 2,  total_num_of_control_points);
            affected_points.push_back(controlManager->getControlPointByID(prev_ID));

        }
        else if (ID % 3 == 0) // This is ID for p3
        {
            auto next_ID = getIdInCircularArray(ID + 2,  total_num_of_control_points);
            affected_points.push_back(controlManager->getControlPointByID(next_ID));
        }
    }

    return affected_points;
}


void CoasterTrack::updateMaxPositionAndTimeStep()
{
    this->maxHeightPos = glm::vec3(0, std::numeric_limits<float>::min(), 0);
    for(int i = 0; i < track.size(); i++)
    {
        if(track[i]->maxHeightPos.y > maxHeightPos.y)
        {
            this->maxHeightPos = track[i]->maxHeightPos;
            this->maxHeightAccumulatedTimeStep = track[i]->maxHeightAccumulatedTimeStep;
            current_bezier_curve_in_use = i;
        }
    }
//    std::cout << "highest y: " << maxHeightPos.y << "   " << current_bezier_curve_in_use << std::endl;
}

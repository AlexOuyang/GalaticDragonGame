//
//  Control.cpp
//  BezierCurve
//
//  Created by Chenxing Ouyang on 5/16/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#include "Window.h"
#include "Control.h"



Control::ControlManager::~ControlManager()
{
    for(int i = 0; i < controlPoints.size(); i++) delete controlPoints[i];
}

// Create a control point of type Control and push into the vector of controlPoints
Control::ControlObj * Control::ControlManager::createControlPoint(Control::ControlType type, glm::vec3 pos)
{
    current_id_taken++;
    // Id always starts at 1 since 0 is for the black background
    ControlObj * control_temp = new ControlObj(current_id_taken, type, glm::vec4(pos, 1));
    controlPoints.push_back(control_temp);
    return control_temp;
}

// Returns true if any of the control points are selected, false otherwise
bool Control::ControlManager::hasControlPointSelected()
{
    return (current_id_selected > 0 && current_id_selected <= current_id_taken);
}

// Set the currently selected id to be the current control point
void Control::ControlManager::setCurrentlySelectedID(int ID)
{
    current_id_selected = ID;
//    std::cout << "Current ID in use: " << current_id_selected << std::endl;
}

// Get the control point by id, id starts at 1
Control::ControlObj * Control::ControlManager::getControlPointByID(int ID)
{
    ID -= 1;
    return (ID > -1 && ID < controlPoints.size()) ? controlPoints[ID] : nullptr;
}



// Get the currently selected control point
Control::ControlObj * Control::ControlManager::getCurrentlySelectedControlPoint()
{
    return getControlPointByID(current_id_selected);
}


unsigned int Control::ControlManager::getNumOfControlPoints()
{
    return controlPoints.size();
}




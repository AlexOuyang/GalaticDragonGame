//
//  Control.h
//  BezierCurve
//
//  Created by Chenxing Ouyang on 5/16/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//

#ifndef Control_h
#define Control_h

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

namespace Control
{
    
    // Defins the type of control points
    enum ControlType
    {
        CONTROL_POINT,
        ANCHOR_POINT,
    };
    
    
    // Defines a control point for anchor and control point of a beizer curve
    class ControlObj
    {
    public:
        unsigned int ID;
        Control::ControlType type;
        glm::vec4 pos;
        
        ControlObj (unsigned int ID, Control::ControlType type, glm::vec4 pos):
        ID(ID),
        type(type),
        pos(pos)
        {}
    };
    
    
    // Abstracts away the Control objects and manages all Control objects in the world
    class ControlManager
    {
    private:
        std::vector<ControlObj*> controlPoints;
        unsigned int current_id_taken; // This id is incremented as new control points are created
        unsigned int current_id_selected; // This marks the current id in use
        
    public:
        ControlManager():
        current_id_taken(0),
        current_id_selected(0)
        {}
        
        ~ControlManager();
        
        // Create a control point of type Control and push into the vector of controlPoints and return it.
        ControlObj * createControlPoint(Control::ControlType type, glm::vec3 pos);
        
        // Returns true if any of the control points are selected, false otherwise
        bool hasControlPointSelected();
        
        // Set the currently selected id to be the current control point
        void setCurrentlySelectedID(int ID);
        
        // Get the control point by id, id starts at 1
        ControlObj * getControlPointByID(int ID);
        
        // Get the currently selected control point
        ControlObj * getCurrentlySelectedControlPoint();
        
        unsigned int getNumOfControlPoints();
        
        
    };
    
}

#endif /* Control_h */



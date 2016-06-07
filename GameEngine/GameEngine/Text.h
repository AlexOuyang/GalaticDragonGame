//
//  Text.h
//  GameEngine
//
//  Created by Chenxing Ouyang on 6/7/16.
//  Copyright © 2016 CSE167. All rights reserved.
//

#ifndef Text_h
#define Text_h

#include <iostream>
#include <map>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Shader.h"



void setUpText();

void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);


#endif /* Text_h */

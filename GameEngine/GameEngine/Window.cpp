
#include "shader.h"
#include "Light.h"
#include "window.h"
#include "OBJObject.h"
#include "Node.h"
#include "skybox.h"
#include "BezierCurve.h"
#include "Control.h"
#include "CoasterTrack.h"
#include "SSAO.h"
#include "AudioManager.h"
#include "AsteroidManager.h"
#include "Dragon.h"
#include "BoundingBox.h"
#include "Text.h"

//defined static member variables
int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;
glm::mat4 Window::V_SSAO;

float rotation_scale = 100.0f;
float translation_scale = 0.04f;
float zoom_scale = 0.5f;
float zoom_max = 600;
float zoom_offset = 400;
float zoom_sum = 0;

bool rotate_spot_light = false;

// Used for mouse trackball
glm::vec3 lastPoint;
glm::vec3 curPoint;
bool left_button_clicked = false;
bool right_button_clicked = false;
bool scroll_back = false;

// Used for dragging control points
glm::vec2 last_cursor_position;
glm::vec2 curr_cursor_position;
float cursor_dragging_speed = 0.01f;

ControlManager * controlManager = nullptr;
Skybox * skybox = nullptr;
Dragon * dragon = nullptr;
OBJObject * castle = nullptr;
AsteroidGroup * asteroidGroup = nullptr;

// Keep track of the scoring
std::string score;

//SSAO Light Properties
glm::vec3 lightPos = glm::vec3(-3.0, 10.0, 0.0);
//glm::vec3 lightPos = glm::vec3(3.0f, 0, 0);
//glm::vec3 lightColor = glm::vec3(0.9, 0.9, 0.9);
//glm::vec3 lightColor = glm::vec3(1, 0, 0);
glm::vec3 lightColor = glm::vec3(0, 0, 0);
//glm::vec3 lightColor = glm::vec3(1,1,1);
//glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);
//glm::vec3 lightColor = glm::vec3(1.0, 1.0, 0.2);

bool pauseGame = false;

bool collided = false;

bool drawSSAO = true;

bool moveLeft = false;
bool moveRight = false;
bool moveUp = false;
bool moveDown = false;

bool cinematicStart = true;
bool dragonDead = false;

bool debugMode = false;

enum MouseActions
{
    NONE,
    LEFT_BUTTON_PRESSED,
    RIGHT_BUTTON_PRESSED,
};

MouseActions currentMouseAction = NONE; // Set Mouse action to none by default

// Control mode designates the mouse control to either the model or lights
enum ControlMode
{
    MODEL_CONTROL,
    CAMERA_CONTROL,
    SPOT_LIGHT_CONTROL,
    DIRECTIONAL_LIGHT_CONTROL,
    POINT_LIGHT_CONTROL,
};

ControlMode currentControlMode = CAMERA_CONTROL; // Set Mouse action to none by default

// Control camera view
enum CameraView
{
    ORIGINAL_VIEW,
    CENTER_VIEW,
    ZOOMEDOUT_VIEW,
};

CameraView camera_view = CENTER_VIEW;

// Shader programs
GLint phongShaderProgram;
GLint skyboxShaderProgram;
GLint bezierCurveShaderProgram;
GLint selectionBufferShaderProgram;
GLint envirMappingShaderProgram;
GLint boundingBoxShaderProgram;


// Default camera parameters
//glm::vec3 cam_pos(0, 0, 10);		            // e  | Position of camera

//glm::vec3 cam_pos(0,-1,0.5);    //game pos of cam (ZOOMED IN)
glm::vec3 cam_pos(0,-3,4.5);    //game pos of cam (ZOOMED OUT)
//glm::vec3 cam_pos(0.0f,-6.5f,3.0f);


glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

void Window::initialize_objects()
{
    controlManager = new ControlManager();
    
    // Initialize audio manager for audio support
    bool temp = AudioManager::init();
    std::cout << "AudioManager: " << temp << std::endl;
    AudioManager::play_background_music();
    
    // Load the shader program. Similar to the .obj objects, different platforms expect a different directory for files
    phongShaderProgram = LoadShaders("./shaders/toon_shader.vert",
                                     "./shaders/toon_shader.frag");
    skyboxShaderProgram = LoadShaders("./shaders/shader_skybox.vert",
                                      "./shaders/shader_skybox.frag");
    bezierCurveShaderProgram = LoadShaders("./shaders/shader_bezier_curve.vert",
                                           "./shaders/shader_bezier_curve.frag");
    selectionBufferShaderProgram = LoadShaders("./shaders/shader_selection_buffer.vert",
                                               "./shaders/shader_selection_buffer.frag");
    envirMappingShaderProgram = LoadShaders("./shaders/shader_environmental_mapping.vert",
                                            "./shaders/shader_environmental_mapping.frag");
    boundingBoxShaderProgram = LoadShaders("./shaders/shader_bounding_box.vert",
                                           "./shaders/shader_bounding_box.frag");
    // Create shaders for SSAO
    SSAO::init(lightPos, lightColor, SSAO::FOG_MODE);
    
    
    // Set up skybox after creating shaders
    skybox = new Skybox(zoom_max - zoom_offset/2.0f,
                        "../../Textures/Skybox/mp_orbital/right.ppm",
                        "../../Textures/Skybox/mp_orbital/left.ppm",
                        "../../Textures/Skybox/mp_orbital/top.ppm",
                        "../../Textures/Skybox/mp_orbital/bottom.ppm",
                        "../../Textures/Skybox/mp_orbital/back.ppm",
                        "../../Textures/Skybox/mp_orbital/front.ppm");
    
    
    // Set up light for phong shader
    Light::setup();
    Light::useLight(Light::DIRECTIONAL_LIGHT);
    
    
    // Hero of the game
    dragon = new Dragon("../../Models/dragon_2_body.obj",
                        "../../Models/dragon_2_left_wing.obj",
                        "../../Models/dragon_2_right_wing.obj");
    SSAO::add_obj(dragon->body);
    SSAO::add_obj(dragon->leftWing);
    SSAO::add_obj(dragon->rightWing);
    
    // Create asteroid group
    int num_of_asteroids = 60;
    float bound_top = 50;
    float bound_down = -15;
    float bound_left = -10;
    float bound_right = 10;
    float bound_z_pos = 10;
    float bound_z_neg = -10;
    asteroidGroup = new AsteroidGroup(num_of_asteroids, bound_top, bound_down,
                                      bound_left, bound_right, bound_z_pos, bound_z_neg);
    for (int i = 0; i < asteroidGroup->asteroids.size(); i++)
        SSAO::add_obj(asteroidGroup->asteroids[i]);
    
    
    
    setUpText();
    
    // Create castle
    //    castle = new OBJObject("../../Models/castle.obj");
    //    castle->rotate(90.0f,glm::vec3(1.0f,0.0f,0.0f));
    //    castle->scale(10);
    //    castle->translate(0, -2, 0);
    //    SSAO::add_obj(castle);
    
    AudioManager::play_roar();
}


void Window::clean_up()
{
    AudioManager::close();
    delete dragon;
    delete castle;
    delete asteroidGroup;
    delete controlManager;
    glDeleteProgram(phongShaderProgram);
    glDeleteProgram(skyboxShaderProgram);
    glDeleteProgram(bezierCurveShaderProgram);
    glDeleteProgram(envirMappingShaderProgram);
    glDeleteProgram(selectionBufferShaderProgram);
    glDeleteProgram(boundingBoxShaderProgram);
    SSAO::delete_shaders();
}

GLFWwindow* Window::create_window(int width, int height)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    
    // 4x antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Create the GLFW window
    GLFWwindow* window = glfwCreateWindow(width, height, window_title, glfwGetPrimaryMonitor(), NULL);
    
    // Check if the window could not be created
    if (!window)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return NULL;
    }
    
    // Make the context of the window
    glfwMakeContextCurrent(window);
    
    // Set swap interval to 1
    glfwSwapInterval(1);
    
    // Get the width and height of the framebuffer to properly resize the window
    glfwGetFramebufferSize(window, &width, &height);
    // Call the resize callback to make sure things get drawn immediately
    Window::resize_callback(window, width, height);
    
    return window;
}

void Window::resize_callback(GLFWwindow* window, int w, int h)
{
    
    Window::width = w;
    Window::height = h;
    //reset the viewport size to avoid bad scaling when enlarging window
    glViewport(0, 0, w, h);
    
    // The initial camera view set up
    Window::P = glm::perspective(45.0f, (float)w / (float)h, 0.1f, 1000.0f);
    Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    
    // Reinitiatialize ssao on resize
    //    SSAO::re_init();
}

void Window::display_callback(GLFWwindow* window)
{
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    /*====== Draw Sybox ======*/
    //    skybox->draw(skyboxShaderProgram);
    
    /*====== Draw SSAO ======*/
    if(drawSSAO)
    {
        SSAO::draw();
    }
    /*====== Draw Phong Shader =======*/
    else
    {
        /*====== Draw Light ======*/
        glUseProgram(phongShaderProgram);
        // Bind cameraPosition, or 'eye' to the shaderProgram
        glUniform3fv(glGetUniformLocation(phongShaderProgram, "eye"), 1, &cam_pos[0]);
        // Bind light to the shaderProgram
        Light::bindDirectionalLightToShader(phongShaderProgram);
        
        /*====== Draw Game Objects ======*/
        dragon->body->draw(phongShaderProgram);
        dragon->leftWing->draw(phongShaderProgram);
        dragon->rightWing->draw(phongShaderProgram);
        
        for (int i = 0; i < asteroidGroup->asteroids.size(); i++)
            asteroidGroup->asteroids[i]->draw(phongShaderProgram);
        
        /*===== Draw Bounding Boxes Via Wireframe ======*/
        
        if (debugMode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
            glUseProgram(boundingBoxShaderProgram);
            // Bind cameraPosition, or 'eye' to the shaderProgram
            glUniform3fv(glGetUniformLocation(boundingBoxShaderProgram, "eye"), 1, &cam_pos[0]);
            // Bind light to the shaderProgram
            Light::bindDirectionalLightToShader(boundingBoxShaderProgram);
            dragon->body->drawBoundingBox(boundingBoxShaderProgram);
            for (int i = 0; i < asteroidGroup->asteroids.size(); i++)
                asteroidGroup->asteroids[i]->drawBoundingBox(boundingBoxShaderProgram);
            
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
    }
    
    // Render score and logo on screen
    score = (dragonDead) ? score : "Score: " + std::to_string(asteroidGroup->numOfAsteroidsPassed);
    renderText(score, 620.0f, 560.0f, 0.8f, glm::vec3(0.0f, 0.0f, 0.0f));
    if (dragonDead)
        renderText("Game Over", 25.0f, 25.0f, 1.0f, glm::vec3(0.05f, 0.05f, 0.05f));
    else
        renderText("Galactic Dragon", 25.0f, 25.0f, 1.0f, glm::vec3(0.05f, 0.05f, 0.05f));
    
    // Swap buffers
    glfwSwapBuffers(window);
    
    // Gets events, including input such as keyboard and mouse or window resizing
    glfwPollEvents();
    
}


void Window::idle_callback()
{
    if (!pauseGame)
    {
        asteroidGroup->update();
        //    std::cout << "Num of Asteroids dodged: " << asteroidGroup->numOfAsteroidsPassed << std::endl;
        
        if(cinematicStart)
        {
            if(cam_pos.z <= 0.1f)
            {
                cinematicStart = false;
            }
            else
            {
                cam_pos.z -= 0.1f;
                cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
                Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
                
            }
        }
        
        if(dragonDead)
        {
            if(cam_pos.z > -40.0f)
            {
                cam_pos.z -= 0.05f;
                cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
                Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
            }
        }
        
        
        if (!collided)
        {
            dragon->update(moveLeft, moveRight, moveUp, moveDown);
            
            // Check for collision
            BoundingBox * other;
            for (int i = 0; i < BoundingBox::boundingBoxes.size(); i++)
            {
                other = BoundingBox::boundingBoxes[i];
                collided = dragon->body->onCollision(other);
                //            std::cout << "Collided: " << collided << std::endl;
                if (collided) {
                    AudioManager::play_death();
                    dragonDead = true;
                    return;
                }
            }
        }
        else
        {
            dragon->falling();
        }
    }
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    /* Super important, since osx retina display has more pixel we need to multiply by 2 */
    xpos *= osx_retina_pixel_multiplier;
    ypos *= osx_retina_pixel_multiplier;
    
    //        std::cout << "x: " << xpos << "   y: " << ypos << std::endl;
    
    glm::vec3 direction;
    float rot_angle;
    
    switch(currentMouseAction)
    {
        case LEFT_BUTTON_PRESSED:
            
            if (!left_button_clicked)
            {
                // Map the mouse position to a logical sphere location.
                // Keep it in the class variable lastPoint.
                lastPoint = trackBallMapping(xpos, ypos);
                
                last_cursor_position = glm::vec2(xpos, -ypos);
                
                // Make sure we are modifying the MODELVIEW matrix.
                left_button_clicked = true;
            }
            else
            {
                
                // Drag control points
                if (controlManager->hasControlPointSelected())
                {
                    curr_cursor_position = glm::vec2(xpos, -ypos);
                    glm::vec2 cursor_displacement = curr_cursor_position - last_cursor_position;
                    
                    
                    drag_control_point(cursor_displacement);
                    
                    last_cursor_position = curr_cursor_position;
                }
                else // Rotate Camera
                {
                    curPoint = trackBallMapping(xpos, ypos); // Map the mouse position to a logical sphere location.
                    direction = curPoint - lastPoint;
                    float velocity =glm::length(direction); // make velocity slower
                    glm::vec3 rot_Axis;
                    rot_Axis = glm::cross(lastPoint, curPoint);
                    rot_angle = velocity * rotation_scale;
                    
                    
                    mouse_drag_to_rotate(rot_angle, rot_Axis);
                    
                    lastPoint = curPoint;
                }
                
            }
            
            break;
            
        case RIGHT_BUTTON_PRESSED:
            
            if (!right_button_clicked)
            {
                // Map the mouse position to a logical sphere location.
                // Keep it in the class variable lastPoint.
                lastPoint = glm::vec3(xpos, -ypos, 0.0f);
                right_button_clicked = true;
            }
            else
            {
                curPoint = glm::vec3(xpos, -ypos, 0.0f);
                direction = (curPoint - lastPoint) * translation_scale;
                
                mouse_drag_to_translate(direction);
                
                lastPoint = curPoint;
            }
            
            
            break;
        case NONE:
        default:
            break;
    }
    
}


void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    int ctl = GLFW_MOD_CONTROL;
    
    // Right button is simply mousepad click
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && (mods & ctl) != ctl)
    {
        currentMouseAction = LEFT_BUTTON_PRESSED;
        
        /*======= Draw selection buffer when left button is clicked ====*/
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        /* Super important, since osx retina display has more pixel we need to multiply by 2 */
        xpos *= osx_retina_pixel_multiplier;
        ypos *= osx_retina_pixel_multiplier;
        // Only call selection buffer on mouse down
        selection_buffer_click(xpos, ypos);
        
    }
    // Right button is mousepad click + control
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && (mods & ctl) == ctl)
    {
        currentMouseAction = RIGHT_BUTTON_PRESSED;
        //        std::cout << "right clicked" << std::endl;
    }
    else
    {
        //        std::cout << "Stopped clicking" << std::endl;
        currentMouseAction = NONE;
        left_button_clicked = false;
        right_button_clicked = false;
    }
}


void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mouse_scroll(xoffset, yoffset);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Check for a key press
    if (action == GLFW_PRESS)
    {
        // Check if escape was pressed
        if (key == GLFW_KEY_ESCAPE)
        {
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        
        int shift = GLFW_MOD_SHIFT;
        
        if (key == GLFW_KEY_S){
            
        }
        
        if (key == GLFW_KEY_P)
        {
            //            collided = false;
            pauseGame = !pauseGame;
        }
        
        if (key == GLFW_KEY_A)
        {
            AudioManager::play_roar();
        }
        
        if (key == GLFW_KEY_R)
        {

        }
        
        
        
        /*====== Camera Controls =======*/
        
        if (key == GLFW_KEY_C)
        {
            change_cam();
        }
        
        // Toggle between fog mode, cartoon mode and grayscale mode
        if (key == GLFW_KEY_T && (mods & shift) == shift)
        {
            drawSSAO = !drawSSAO;
            //            if(drawSSAO)    glClearColor(0, 0, 0, 0); // If draw ssao, make background black
            //            else    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // If debug mode, make background white
        }
        if (key == GLFW_KEY_T && (mods & shift) != shift)
        {
            if(SSAO::mode == SSAO::GRAY_SCALE_MODE)
                SSAO::re_init(SSAO::FOG_MODE);
            else if (SSAO::mode == SSAO::FOG_MODE)
                SSAO::re_init(SSAO::GRAY_SCALE_MODE);
        }
        
        if (key == GLFW_KEY_D)
        {
            if (!drawSSAO){
                debugMode = !debugMode;
            }
        }
        
        // Draw LSD mode
        if (key == GLFW_KEY_L)
        {
            if(SSAO::mode == SSAO::RAINBOW_MODE)
                SSAO::re_init(SSAO::FOG_MODE);
            else
                SSAO::re_init(SSAO::RAINBOW_MODE);
        }
    }
    
    // Holding down key
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        /*====== Hero Controls =======*/
        
        // Arrow keys that move the hero character
        if (key == GLFW_KEY_LEFT)
        {
            moveLeft = true;
        }
        if (key == GLFW_KEY_RIGHT)
        {
            moveRight = true;
        }
        if (key == GLFW_KEY_UP)
        {
            moveUp = true;
        }
        if (key == GLFW_KEY_DOWN)
        {
            moveDown = true;
        }
    }
    
    if(action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_LEFT)
        {
            moveLeft = false;
        }
        if (key == GLFW_KEY_RIGHT)
        {
            moveRight = false;
        }
        if (key == GLFW_KEY_UP)
        {
            moveUp = false;
        }
        if (key == GLFW_KEY_DOWN)
        {
            moveDown = false;
        }
    }
    
}



/*================ Private Methods ==================*/


glm::vec3 Window::trackBallMapping(const double &x, const double &y)    // The CPoint class is a specific Windows class. Either use separate x and y values for the mouse location, or use a Vector3 in which you ignore the z coordinate.
{
    glm::vec3 v;     // Vector v is the synthesized 3D position of the mouse location on the trackball
    float d;         // this is the depth of the mouse location: the delta between the plane through the center of the trackball and the z position of the mouse
    v.x = (2.0 * x - width) / width;   // this calculates the mouse X position in trackball coordinates, which range from -1 to +1
    v.y = (height - 2.0 * y) / height;   // this does the equivalent to the above for the mouse Y position
    v.z = 0.0;   // initially the mouse z position is set to zero, but this will change below
    d = glm::length(v);    // this is the distance from the trackball's origin to the mouse location, without considering depth (=in the plane of the trackball's origin)
    d = (d<1.0) ? d : 1.0;   // this limits d to values of 1.0 or less to avoid square roots of negative values in the following line
    v.z = sqrtf(1.001 - d * d);  // this calculates the Z coordinate of the mouse position on the trackball, based on Pythagoras: v.z*v.z + d*d = 1*1
    glm::normalize(v); // Still need to normalize, since we only capped d, not v.
    return v;  // return the mouse location on the surface of the trackball
}

// Called in cursor_position_callback() to rotate the light or the model based on the control mode
void Window::mouse_drag_to_rotate(float rot_angle, glm::vec3 rot_Axis)
{
    glm::mat4 R_transpose;
    
    switch (currentControlMode) {
        case MODEL_CONTROL:
            break;
        case DIRECTIONAL_LIGHT_CONTROL:
            Light::rotateDirectionalLight(rot_angle, rot_Axis);
            break;
        case POINT_LIGHT_CONTROL:
            Light::orbitPointLight(rot_angle, rot_Axis);
            break;
        case SPOT_LIGHT_CONTROL:
            if (!rotate_spot_light)
                Light::orbitSpotLight(rot_angle, rot_Axis);
            else
                Light::rotateSpotLight(rot_angle, rot_Axis);
            break;
        case CAMERA_CONTROL:
            
            // Rotate the camera based on the equation: V’ = V * R’_transpose
            R_transpose = glm::transpose(glm::rotate(glm::mat4(1.0f), rot_angle / 180.0f * glm::pi<float>(), rot_Axis));
            cam_up = glm::vec4(cam_up, 0) * R_transpose;
            cam_pos = glm::vec4(cam_pos, 0) * R_transpose;
            Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
            
            //            Window::V = V * glm::transpose(glm::rotate(glm::mat4(1.0f), rot_angle / 180.0f * glm::pi<float>(), rot_Axis));
            break;
        default:
            break;
    }
}


// Called in cursor_position_callback() to translate the light or the model based on the control mode
void Window::mouse_drag_to_translate(glm::vec3 dir)
{
    switch (currentControlMode) {
        case MODEL_CONTROL:
            break;
        case DIRECTIONAL_LIGHT_CONTROL:
            break;
        case POINT_LIGHT_CONTROL:
            break;
        case SPOT_LIGHT_CONTROL:
            break;
        case CAMERA_CONTROL:
            break;
        default:
            break;
    }
}

// Called in scroll_callback() to swipe the keypad using two fingers to zoom in and out
void Window::mouse_scroll(float delta_x, float delta_y)
{
    switch (currentControlMode)
    {
        case MODEL_CONTROL:
            //            if (drawBunny) bunny->translate(0.0f, 0.0f, delta_y * zoom_scale);
            break;
        case DIRECTIONAL_LIGHT_CONTROL:
            break;
        case POINT_LIGHT_CONTROL:
            Light::changePointLightRadius(delta_y);
            break;
        case SPOT_LIGHT_CONTROL:
            Light::changeSpotLightRadius(delta_y);
            break;
        case CAMERA_CONTROL:
            
            if (true) {
                // V = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, delta_y * zoom_scale)) * V; // V = V * -T
                
                delta_y *= -1;
                float r_min = 1.0f;
                float r_magnitude = glm::length(cam_pos);
                r_magnitude = ((r_magnitude + delta_y) < r_min) ? r_min : r_magnitude + delta_y;
                cam_pos = r_magnitude * glm::normalize(cam_pos);
                
                Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
                
            }
            
            break;
        default:
            break;
    }
}

// Detect if control points are clicked via selection buffer
void Window::selection_buffer_click(double xpos, double ypos)
{
    // Fix the ypos since glReadPixel read pixel centered at lower left corner,
    // but GLFW pos detection is centered at upper left corner.
    ypos = height - ypos;
    
    //    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(selectionBufferShaderProgram);
    
    unsigned char pix[4];
    glReadPixels(xpos, ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pix);
    unsigned int recovered_ID = (unsigned int)(pix[0]);
    
    controlManager->setCurrentlySelectedID(recovered_ID);
    
    
    //    std::cout << "window height: " << height<< std::endl;
    //    std::cout << "window width: " << width<< std::endl;
    //    std::cout << "x: " << xpos << "   y: " << height-ypos << "   id: " << recovered_ID << std::endl;
}

// Move control points by displacement
void Window::drag_control_point(glm::vec2 displacement)
{
    //    auto clicked_point = controlManager->getCurrentlySelectedControlPoint();
    //    auto affected_control_points = track->getAffectedControlPoints(clicked_point);
    //    if (affected_control_points.size() == 0) return;
    //
    //    // 0.116f is a good constant to make sure control points moves as fast as cursor
    //    float distance_dragging_multiplier = 0.1f * glm::length(cam_pos - cam_look_at);
    //    displacement = displacement * cursor_dragging_speed * distance_dragging_multiplier;
    //
    //    glm::vec3 cam_z = glm::normalize(cam_pos - cam_look_at);
    //    glm::vec3 cam_x = glm::normalize(glm::cross(cam_up, cam_z));
    //    glm::vec3 cam_y = glm::cross(cam_z, cam_x);
    //
    //    if (clicked_point->type == ANCHOR_POINT)
    //    {
    //        for (int i = 0; i < affected_control_points.size(); i++)
    //        {
    //            auto control_point = affected_control_points[i];
    //
    //            if (!control_point) return;
    //
    //            control_point->pos = control_point->pos + glm::vec4(displacement.x * cam_x, 0.0f) + glm::vec4(displacement.y * cam_y, 0.0f);
    //        }
    //    }
    //    else if (clicked_point->type == CONTROL_POINT)
    //    {
    //        clicked_point->pos = clicked_point->pos + glm::vec4(displacement.x * cam_x, 0.0f) + glm::vec4(displacement.y * cam_y, 0.0f);
    //        auto mirror_point = affected_control_points[0];
    //        mirror_point->pos = mirror_point->pos - glm::vec4(displacement.x * cam_x, 0.0f) - glm::vec4(displacement.y * cam_y, 0.0f);
    //    }
    //
    //    track->updateCurve();
    
}

void Window::change_cam()
{
    switch(camera_view){
        case ORIGINAL_VIEW:
            std::cout << "CENTER_VIEW" << std::endl;
            cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
            cam_pos = glm::vec3(0.0f,-3.0f,0.1f);
            Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
            camera_view = CENTER_VIEW;
            break;
        case CENTER_VIEW:
            std::cout << "ZOOMEDOUT_VIEW" << std::endl;
            cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
            cam_pos = glm::vec3(0.0f,-6.5f,3.0f);
            Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
            camera_view = ZOOMEDOUT_VIEW;
            break;
        case ZOOMEDOUT_VIEW:
            std::cout << "ORIGINAL_VIEW" << std::endl;
            cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
            cam_pos = glm::vec3(0.0f,-3.0f,1.5f);
            Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
            camera_view = ORIGINAL_VIEW;
            break;
        default:
            break;
    }
}

void Window::rotate_cam(float rotAngle, glm::vec3 rotAxis)
{
    auto rotationMat = glm::rotate(glm::mat4(1.0f), rotAngle / 180.0f * glm::pi<float>(), rotAxis);
    cam_pos = glm::vec4(cam_pos, 0) * rotationMat;
    Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}
//
//void Window::cinematic_cam(glm::vec3 current_cam_pos, glm::vec3 dest_cam_pos)
//{
//    cam_pos +
//    glm::vec3(0.0f,-3.0f,1.5f);
//
//    Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
//}
//void Window::change_cam_look_at(glm::vec3 vec)
//{
//    cam_look_at += vec;
//    Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
//}



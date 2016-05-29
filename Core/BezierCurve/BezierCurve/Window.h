#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Window
{
private:
    constexpr const static float osx_retina_cursor_pos_offset = 2.0f;
    // Called in display_callback to display the scene
    static void display_full_screen();
    static glm::vec3 trackBallMapping(const double &x, const double &y);
    static void camera_view_setup(int width, int height);
    static void mouse_drag_to_translate(glm::vec3 dir);
    static void mouse_drag_to_rotate(float rot_angle, glm::vec3 rot_Axis);
    static void mouse_scroll(float xoffset, float yoffset);
    static void selection_buffer_click(double xpos, double ypos);
    static void drag_control_point(glm::vec2 displacement);

public:
	static int width;
	static int height;
	static glm::mat4 P;
	static glm::mat4 V;
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif

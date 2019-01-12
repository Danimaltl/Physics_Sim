#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h> 
//#include <SFML/Graphics.hpp>
//#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
//#include <SFML/Main.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <string>
#include <assert.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

//extern sf::Window window;

extern unsigned int sWidth;
extern unsigned int sHeight;


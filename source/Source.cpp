#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <utility>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifdef _WIN32
#include <Windows.h>
#endif
#include "shader.hpp"
#include "control.hpp"
#include "vboIndexer.hpp"
#include "triangleFace.hpp"

//extern "C" {
//    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

GLFWwindow* window;
const int SCR_WIDTH = 1920;
const int SCR_HEIGHT = 1080;
glm::vec3 pos = glm::vec3(0.0f,0.0f,2.0f);

GLuint infinitevao, infinitevbo, infinitenorm;

GLuint vaot, vbot, nbot, ebot;
GLuint fbo, resTex, resDep;
GLuint mul_fbo, mul_resTex, mul_resDep;
ImVec2 vMin, vMax;
double xpos, ypos; //For mouse input
int takeImage = 0;
bool AAFlag = false, prevAAFlag = false;
const int xrange = 5;
const int yrange = 3;
const int zrange = 5;
std::vector<glm::vec3> finalTris[xrange * yrange * zrange];
std::vector<unsigned short> indices[xrange * yrange * zrange];///<Vector to store indicies of triangles to be plotted
std::vector<glm::vec3> indexed_vertices[xrange * yrange * zrange];///<Vector to stored indexed vertices
std::vector<glm::vec3> indexed_normals[xrange * yrange * zrange];///<Vector to stored indexed normals
int infinite = 0;
std::vector<glm::vec3> infiniteTris[28];
std::vector<glm::vec3> infiniteNorm[28];
glm::vec3 prevPos;
int helpFlag = 1;
std::string helpString = "H : Toggle Help window\n"
"T : Take Screenshot\n"
"I : Toggle flat shading and infinite generation\n"
"Q : Toggle anti-aliasing\n"
"ESC : Exit"
;

int triTablea[256][16] =
{ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} };

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    glfwGetCursorPos(window, &xpos, &ypos);
}
void CheckFBOStatus(GLuint fbo, GLenum target) {
    GLenum fboStatus = glCheckNamedFramebufferStatus(fbo, target);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        switch (fboStatus)
        {
        case GL_FRAMEBUFFER_UNDEFINED:
            std::cout << "No window\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cout << "Check attachment status\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cout << "Attach attachments\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cout << "Enable attachments\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cout << "Check buffer from glReadBuffer exists in FBO\n";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cout << "Change formats\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            std::cout << "Number of samples for each multisample is same?\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            std::cout << "Number of layers for each multisample is same?\n";
        default:
            break;
        }
    }
    else
        std::cout << "Complete FBO :D\n";
    return;
}
int initialize() {
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();
    const char* glsl_version = "#version 450";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Borderless in release mode, don't fool around unless you want to sign out or restart
#if defined(_DEBUG)
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Marching Cubes", NULL, NULL);
#else
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    window = glfwCreateWindow(mode->width, mode->height, "Marching Cubes", primary, NULL);
#endif
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);//For Key Input
    glfwPollEvents();//Continously Checks For Input
    glfwSetCursorPos(window, 1920 / 2, 1080 / 2);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    glGenVertexArrays(1, &infinitevao);
    glBindVertexArray(infinitevao);

    glGenBuffers(1, &infinitevbo);
    glBindBuffer(GL_ARRAY_BUFFER, infinitevbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &infinitenorm);
    glBindBuffer(GL_ARRAY_BUFFER, infinitenorm);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //Rendering into FBO
    glCreateFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &resTex);
    glBindTexture(GL_TEXTURE_2D, resTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, SCR_WIDTH, SCR_HEIGHT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &resDep);
    glBindTexture(GL_TEXTURE_2D, resDep);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, SCR_WIDTH, SCR_HEIGHT);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resTex, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, resDep, 0);

    static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    CheckFBOStatus(fbo, GL_DRAW_FRAMEBUFFER);

    //For multisample FBO
    glGenFramebuffers(1, &mul_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mul_fbo);
    glGenTextures(1, &mul_resTex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mul_resTex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 1, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mul_resTex, 0);
    glGenTextures(1, &mul_resDep);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mul_resDep);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 1, GL_DEPTH_COMPONENT32F, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, mul_resDep, 0);
    glDrawBuffers(1, drawBuffers);
    CheckFBOStatus(mul_fbo, GL_DRAW_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    return 1;
}
void takeSS() {
    //Saving as .TGA
    int rowSize = ((SCR_WIDTH * 3 + 3) & ~3);
    int dataSize = SCR_HEIGHT * rowSize;
    //std::cout << "Image size " << dataSize << "\n";
    unsigned char* data = new unsigned char[dataSize];
#pragma pack (push,1) //Aligns structure members on 1-byte boundaries, or on their natural alignment boundary, whichever is less.
    struct
    {
        unsigned char identsize;
        unsigned char cmaptype;
        unsigned char imagetype;
        short cmapstart;
        short cmapsize;
        unsigned char cmapbpp;
        short xorigin;
        short yorigin;
        short width;
        short height;
        unsigned char bpp;
        unsigned char descriptor;
    }tga_header;
#pragma pack (pop)  //Same as #pragma pack

    glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, data);
    memset(&tga_header, 0, sizeof(tga_header));
    tga_header.imagetype = 2;
    tga_header.width = (short)SCR_WIDTH;
    tga_header.height = (short)SCR_HEIGHT;
    tga_header.bpp = 24;
    FILE* f_out = fopen("ScreenShot.tga", "wb");
    fwrite(&tga_header, sizeof(tga_header), 1, f_out);
    fwrite(data, dataSize, 1, f_out);
    fclose(f_out);

    return;
}
void changeSamples(GLsizei flag) {
    glBindFramebuffer(GL_FRAMEBUFFER, mul_fbo);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mul_resTex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, flag, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mul_resTex, 0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mul_resDep);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, flag, GL_DEPTH_COMPONENT32F, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, mul_resDep, 0);
    static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    CheckFBOStatus(mul_fbo, GL_DRAW_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return;
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
            takeSS();
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
            AAFlag= !AAFlag;
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE)
            infinite = (infinite+1)%3;
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE)
            helpFlag = !helpFlag;
    return;
}
void cleanUp() {

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    glDeleteFramebuffers(1, &fbo);
    glDeleteFramebuffers(1, &mul_fbo);
    glDeleteVertexArrays(1, &vaot);
    glDeleteBuffers(1, &vbot);
    glDeleteBuffers(1, &nbot);
    glDeleteBuffers(1, &ebot);

    glDeleteTextures(1, &mul_resTex);
    glDeleteTextures(1, &mul_resDep);
    glDeleteTextures(1, &resDep);
    glDeleteTextures(1, &resTex);

    indexed_normals->clear();
    indexed_vertices->clear();
    indices->clear();
    finalTris->clear();

    return;
}

void calcTris() {
    std::vector<triangleFace> faces;
    for (int i = 0;i < xrange * yrange * zrange;i++) {
        indexVBO(finalTris[i], indices[i], indexed_vertices[i]);
        finalTris[i].clear();
        for (int j = 0;j < indices[i].size();j += 3) {
            triangleFace temp = triangleFace(j / 3, indices[i][j], indices[i][j + 1], indices[i][j + 2], indexed_vertices[i]);
            faces.push_back(temp);
        }
        for (int j = 0;j < indexed_vertices[i].size();j++) {
            indexed_normals[i].push_back(glm::vec3(0.0f));
        }
        for (int j = 0;j < faces.size();j++) {
            indexed_normals[i][faces[j].indices[0]] += faces[j].normal;
            indexed_normals[i][faces[j].indices[1]] += faces[j].normal;
            indexed_normals[i][faces[j].indices[2]] += faces[j].normal;
        }
        faces.clear();
    }
    glGenBuffers(1, &vbot);
    glGenBuffers(1, &nbot);
    glGenVertexArrays(1, &vaot);
    glBindVertexArray(vaot);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbot);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, nbot);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &ebot);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebot);
}


void generateTriangles(int flag, int ind, float xOff, float yOff, float zOff, glm::vec3 pos) {
    const int numCubes = 32; //Change in compute shader too
    float length = 1.0f;
#if defined(_DEBUG)
    std::cout << "-----------------\n";
    LARGE_INTEGER frequency;
    LARGE_INTEGER t1, t2, t3, t4;
    double elapsedTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);
#endif
    if (!flag) {  
        for (int i = 0;i < xrange;i++) {
            for (int j = 0;j < yrange;j++) {
                for (int k = 0;k < zrange;k++) {
#if defined(_DEBUG)
                    QueryPerformanceCounter(&t3);
#endif

                    Shader computeShader("marchShader.computeShader.glsl");
                    computeShader.use();
                    computeShader.setFloat("inc", length / numCubes);
                    computeShader.setInt("numCubes", numCubes);
                    computeShader.setVec3("stPoint", glm::vec3(i - 1, j - 1, k - 1));

                    GLuint  data_buffer[2];
                    int NUM_ELEMENTS = numCubes * numCubes * numCubes * 15;

                    glGenBuffers(2, data_buffer);

                    glBindBuffer(GL_SHADER_STORAGE_BUFFER, data_buffer[0]);
                    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_ELEMENTS * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);

                    glBindBuffer(GL_SHADER_STORAGE_BUFFER, data_buffer[1]);
                    glBufferData(GL_SHADER_STORAGE_BUFFER, 256 * 16 * sizeof(int), NULL, GL_DYNAMIC_COPY);

                    glShaderStorageBlockBinding(computeShader.ID, 0, 0);
                    glShaderStorageBlockBinding(computeShader.ID, 1, 1);

                    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, data_buffer[0], 0, sizeof(glm::vec4) * NUM_ELEMENTS);

                    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, data_buffer[1], 0, sizeof(int) * 256 * 16);
                    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * 256 * 16, triTablea);

                    computeShader.use();

                    glDispatchCompute(numCubes/8, numCubes/8, numCubes/8);

                    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
                    glFinish();

                    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, data_buffer[0], 0, sizeof(glm::vec4) * NUM_ELEMENTS);

                    glm::vec4* ptr = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4) * NUM_ELEMENTS, GL_MAP_READ_BIT);
#if defined(_DEBUG)
                    QueryPerformanceCounter(&t4);
                    elapsedTime = (t4.QuadPart - t3.QuadPart) * 1000.0 / frequency.QuadPart;
                    std::cout << elapsedTime << "ms to generate vertices for chunk at( "<<i<<","<<j<<"," << k<<" )\n";
                    QueryPerformanceCounter(&t3);
#endif
                    std::vector<glm::vec3> tris;

                    while (NUM_ELEMENTS--) {
                        if ((*ptr).x) {
                            tris.push_back(glm::vec3((*ptr).y, (*ptr).z, (*ptr).w));
                            ptr++;
                        }
                        else {
                            ptr += 1;
                        }
                    }
#if defined(_DEBUG)
                    QueryPerformanceCounter(&t4);
                    elapsedTime = (t4.QuadPart - t3.QuadPart) * 1000.0 / frequency.QuadPart;
                    std::cout << elapsedTime << "ms to iterate over generated vertices\n";
                    QueryPerformanceCounter(&t3);
#endif
                    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
                    glDeleteBuffers(2, data_buffer);
                    finalTris[i * zrange * yrange + j * zrange + k].insert(finalTris[i * zrange * yrange + j * zrange + k].end(), tris.begin(), tris.end());
                    computeShader.deleteProg();
                }
            }
        }
#if defined(_DEBUG)
        std::cout <<"\n";
        QueryPerformanceCounter(&t2);
        elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
        std::cout << elapsedTime << "ms to generate all chunks\n";
        QueryPerformanceCounter(&t1);
#endif
        calcTris();
#if defined(_DEBUG)
        QueryPerformanceCounter(&t2);
        elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
        std::cout << elapsedTime << "ms to index vertices and calculate normals for each chunk\n";
        std::cout << "-----------------\n";
#endif
    }
    else {
        Shader computeShader("marchShader.computeShader.glsl");
        computeShader.use();
        computeShader.setFloat("inc", length / numCubes);
        computeShader.setInt("numCubes", numCubes);
        //glm::vec3 pos = getPosition();
        pos.x += (xOff);
        pos.y += (yOff);
        pos.z += (zOff);
        computeShader.setVec3("stPoint", pos);

        GLuint  data_buffer[2];
        int NUM_ELEMENTS = numCubes * numCubes * numCubes * 15;

        glGenBuffers(2, data_buffer);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, data_buffer[0]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_ELEMENTS * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, data_buffer[1]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 256 * 16 * sizeof(int), NULL, GL_DYNAMIC_COPY);

        glShaderStorageBlockBinding(computeShader.ID, 0, 0);
        glShaderStorageBlockBinding(computeShader.ID, 1, 1);

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, data_buffer[0], 0, sizeof(glm::vec4) * NUM_ELEMENTS);

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, data_buffer[1], 0, sizeof(int) * 256 * 16);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * 256 * 16, triTablea);

        computeShader.use();
        glDispatchCompute(numCubes/8, numCubes/8, numCubes/8);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glFinish();
#if defined(_DEBUG)
        QueryPerformanceCounter(&t2);
        elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
        std::cout << elapsedTime << "ms to generate vertices for chunk at( " << pos.x << "," << pos.y << "," << pos.z << " )\n";
        QueryPerformanceCounter(&t1);
#endif
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, data_buffer[0], 0, sizeof(glm::vec4) * NUM_ELEMENTS);
        glm::vec4* ptr = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4) * NUM_ELEMENTS, GL_MAP_READ_BIT);
        std::vector<glm::vec3> tris;
        std::vector<glm::vec3> norm;
        int i = 0;
        while (NUM_ELEMENTS) {
            NUM_ELEMENTS -= 3;
            if ((*ptr).x) {
                i += 3;
                tris.push_back(glm::vec3((*ptr).y, (*ptr).z, (*ptr).w));
                ptr++;
                tris.push_back(glm::vec3((*ptr).y, (*ptr).z, (*ptr).w));
                ptr++;
                tris.push_back(glm::vec3((*ptr).y, (*ptr).z, (*ptr).w));
                ptr++;
                glm::vec3 normal = glm::normalize(glm::cross(tris[i-1]-tris[i-3], tris[i - 2] - tris[i - 3]));
                norm.push_back(normal);
                norm.push_back(normal);
                norm.push_back(normal);
            }
            else {
                ptr += 3;
            }
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glDeleteBuffers(2, data_buffer);
        infiniteTris[ind] = tris;
        infiniteNorm[ind] = norm;
        computeShader.deleteProg();
#if defined(_DEBUG)
        QueryPerformanceCounter(&t2);
        elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
        std::cout << elapsedTime << "ms to copy vertices and compute normals\n";
        std::cout << "-----------------\n";
#endif
    }
    return;
}

void draw(Shader baseShader) {
    baseShader.use();
    pos = getPosition();
    computeMatricesFromInputs(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    baseShader.setMat4("mv_matrix", ViewMatrix);
    baseShader.setMat4("proj_matrix", ProjectionMatrix);
    baseShader.setVec3("lightPos", getPosition());
    baseShader.setVec3("cameraDir", getDirection());
    glBindVertexArray(vaot);

    for (int i = 0;i < xrange * yrange * zrange;i++) {
        if (indexed_vertices[i].size()) {
            glBindBuffer(GL_ARRAY_BUFFER, vbot);
            glBufferData(GL_ARRAY_BUFFER, indexed_vertices[i].size() * sizeof(glm::vec3), &indexed_vertices[i][0], GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, nbot);
            glBufferData(GL_ARRAY_BUFFER, indexed_normals[i].size() * sizeof(glm::vec3), &indexed_normals[i][0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebot);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(unsigned short), &indices[i][0], GL_STATIC_DRAW);
            glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_SHORT, (void*)(0));
        }
    }

    return;
}
void updateChunks() {
    glm::vec3 curPos = getPosition();
    float switchDis = 0.75f;
    if ((curPos.x - prevPos.x) > switchDis) {
        prevPos.x += 1.0f;
        for (int i = 0;i < 2;i++)
        {
            for (int j = 0;j < 3;j++)
            {
                for (int k = 0;k < 3;k++) {
                    infiniteTris[i * 3 * 3 + j * 3 + k] = infiniteTris[i * 3 * 3 + j * 3 + k + 9];
                    infiniteNorm[i * 3 * 3 + j * 3 + k] = infiniteNorm[i * 3 * 3 + j * 3 + k + 9];
                }
            }
        }
        for (int i = 2;i < 3;i++)
        {
            for (int j = 0;j < 3;j++)
            {
                for (int k = 0;k < 3;k++) {
                    generateTriangles(1, i * 3 * 3 + j * 3 + k, (float)i - 1.5f, (float)j - 1.5f, (float)k - 1.5f, prevPos);
                }
            }
        }
        return;
    }
    else if ((curPos.x - prevPos.x) < -switchDis) {
        prevPos.x -= 1.0f;
        for (int i = 2;i >0;i--)
        {
            for (int j = 0;j < 3;j++)
            {
                for (int k = 0;k < 3;k++) {
                    infiniteTris[i * 3 * 3 + j * 3 + k] = infiniteTris[i * 3 * 3 + j * 3 + k - 9];
                    infiniteNorm[i * 3 * 3 + j * 3 + k] = infiniteNorm[i * 3 * 3 + j * 3 + k - 9];
                }
            }
        }
        for (int i = 0;i < 1;i++)
        {
            for (int j = 0;j < 3;j++)
            {
                for (int k = 0;k < 3;k++) {
                    generateTriangles(1, i * 3 * 3 + j * 3 + k, (float)i - 1.5f, (float)j - 1.5f, (float)k - 1.5f, prevPos);
                }
            }
        }
        return;
    }
    if ((curPos.y - prevPos.y) > switchDis) {
        prevPos.y += 1.0f;
        for (int i = 0;i < 3;i++)
        {
            for (int j = 0;j < 2;j++)
            {
                for (int k = 0;k < 3;k++) {
                    infiniteTris[i * 3 * 3 + j * 3 + k] = infiniteTris[i * 3 * 3 + j * 3 + k + 3];
                    infiniteNorm[i * 3 * 3 + j * 3 + k] = infiniteNorm[i * 3 * 3 + j * 3 + k + 3];
                }
            }
        }
        for (int i = 0;i < 3;i++)
        {
            for (int j = 2;j < 3;j++)
            {
                for (int k = 0;k < 3;k++) {
                    generateTriangles(1, i * 3 * 3 + j * 3 + k, (float)i - 1.5f, (float)j - 1.5f, (float)k - 1.5f, prevPos);
                }
            }
        }
        return;
    }
    else if ((curPos.y - prevPos.y) < -switchDis) {
        prevPos.y -= 1.0f;
        for (int i = 0;i < 3;i++)
        {
            for (int j = 2;j >0 ;j--)
            {
                for (int k = 0;k < 3;k++) {
                    infiniteTris[i * 3 * 3 + j * 3 + k] = infiniteTris[i * 3 * 3 + j * 3 + k - 3];
                    infiniteNorm[i * 3 * 3 + j * 3 + k] = infiniteNorm[i * 3 * 3 + j * 3 + k - 3];
                }
            }
        }
        for (int i = 0;i < 3;i++)
        {
            for (int j = 0;j < 1;j++)
            {
                for (int k = 0;k < 3;k++) {
                    generateTriangles(1, i * 3 * 3 + j * 3 + k, (float)i - 1.5f, (float)j - 1.5f, (float)k - 1.5f, prevPos);
                }
            }
        }
        return;
    }
    if ((curPos.z - prevPos.z) > switchDis) {
        prevPos.z += 1.0f;
        for (int i = 0;i < 3;i++)
        {
            for (int j = 0;j < 3;j++)
            {
                for (int k = 0;k < 2;k++) {
                    infiniteTris[i * 3 * 3 + j * 3 + k] = infiniteTris[i * 3 * 3 + j * 3 + k + 1];
                    infiniteNorm[i * 3 * 3 + j * 3 + k] = infiniteNorm[i * 3 * 3 + j * 3 + k + 1];
                }
            }
        }
        for (int i = 0;i < 3;i++)
        {
            for (int j = 0;j < 3;j++)
            {
                for (int k = 2;k < 3;k++) {
                    generateTriangles(1, i * 3 * 3 + j * 3 + k, (float)i - 1.5f, (float)j - 1.5f, (float)k - 1.5f, prevPos);
                }
            }
        }
        return;
    }
    else if ((curPos.z - prevPos.z) < -switchDis) {
        prevPos.z -= 1.0f;
        for (int i = 0;i < 3;i++)
        {
            for (int j = 0;j < 3;j++)
            {
                for (int k = 2;k > 0;k--) {
                    infiniteTris[i * 3 * 3 + j * 3 + k] = infiniteTris[i * 3 * 3 + j * 3 + k - 1];
                    infiniteNorm[i * 3 * 3 + j * 3 + k] = infiniteNorm[i * 3 * 3 + j * 3 + k - 1];
                }
            }
        }
        for (int i = 0;i < 3;i++)
        {
            for (int j = 0;j < 3;j++)
            {
                for (int k = 0;k < 1;k++) {
                    generateTriangles(1, i * 3 * 3 + j * 3 + k, (float)i - 1.5f, (float)j - 1.5f, (float)k - 1.5f, prevPos);
                }
            }
        }
        return;
    }
}
int main() {
    if (initialize() < 0)
        return -1;
    pos = getPosition();
    prevPos = pos;
    generateTriangles(0,NULL,NULL,NULL,NULL,glm::vec3(NULL));
    for (int i = 0;i < 3;i++)
    {
        for (int j = 0;j < 3;j++)
        {
            for (int k = 0;k < 3;k++) {
                    generateTriangles(1, i * 3 * 3 + j * 3 + k, (float)i - 1.5f, (float)j - 1.5f, (float)k - 1.5f, prevPos);
            }
        }
    }
    Shader baseShader("vShader.vertexShader.glsl", "fShader.fragmentShader.glsl");
    Shader flatShader("vShaderFlat.vertexShader.glsl", "fShaderFlat.fragmentShader.glsl");
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        cursor_position_callback(window, xpos, ypos);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, mul_fbo);
        //Draw calls here
        if (!infinite) {
            draw(baseShader);
        }
        else if(infinite == 1) {
            updateChunks();           
            flatShader.use();
            computeMatricesFromInputs(window);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::mat4 ProjectionMatrix = getProjectionMatrix();
            glm::mat4 ViewMatrix = getViewMatrix();
            flatShader.setMat4("mv_matrix", ViewMatrix);
            flatShader.setMat4("proj_matrix", ProjectionMatrix);
            flatShader.setVec3("lightPos", getPosition() );
            flatShader.setVec3("cameraDir", getDirection());
            glBindVertexArray(infinitevao);
            glBindBuffer(GL_ARRAY_BUFFER, infinitevbo);
            for (int i = 0;i < 27;i++) {
                if (infiniteTris[i].size()) {
                    glBindBuffer(GL_ARRAY_BUFFER, infinitevbo);
                    glBufferData(GL_ARRAY_BUFFER, infiniteTris[i].size() * sizeof(glm::vec3), &infiniteTris[i][0], GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, infinitenorm);
                    glBufferData(GL_ARRAY_BUFFER, infiniteNorm[i].size() * sizeof(glm::vec3), &infiniteNorm[i][0], GL_STATIC_DRAW);
                    glDrawArrays(GL_TRIANGLES, 0, infiniteTris[i].size());
                }
            }
        }
        else {
            generateTriangles(1, 27, 0, 0, 0, getPosition()-glm::vec3(0.5f));
            flatShader.use();
            computeMatricesFromInputs(window);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::mat4 ProjectionMatrix = getProjectionMatrix();
            glm::mat4 ViewMatrix = getViewMatrix();
            flatShader.setMat4("mv_matrix", ViewMatrix);
            flatShader.setMat4("proj_matrix", ProjectionMatrix);
            flatShader.setVec3("lightPos", getPosition());
            flatShader.setVec3("cameraDir", getDirection());
            glBindVertexArray(infinitevao);
            glBindBuffer(GL_ARRAY_BUFFER, infinitevbo);
            for (int i = 27;i < 28;i++) {
                if (infiniteTris[i].size()) {
                    glBindBuffer(GL_ARRAY_BUFFER, infinitevbo);
                    glBufferData(GL_ARRAY_BUFFER, infiniteTris[i].size() * sizeof(glm::vec3), &infiniteTris[i][0], GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, infinitenorm);
                    glBufferData(GL_ARRAY_BUFFER, infiniteNorm[i].size() * sizeof(glm::vec3), &infiniteNorm[i][0], GL_STATIC_DRAW);
                    glDrawArrays(GL_TRIANGLES, 0, infiniteTris[i].size());
                }
            }
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mul_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);


        
        ImGui::SetNextWindowSize(ImVec2(1920.0f, 1080.0f), 0);
        ImGui::SetNextWindowPos(ImVec2(0.0f,0.0f), 0);
        ImGui::Begin("OpenGL Result");
        {
            ImGui::BeginChild("OpenGL Result");
            ImVec2 wsize = ImGui::GetWindowSize();
            vMin = ImGui::GetWindowContentRegionMin();
            vMax = ImGui::GetWindowContentRegionMax();
            vMin.x += ImGui::GetWindowPos().x;
            vMin.y += ImGui::GetWindowPos().y;
            vMax.x += ImGui::GetWindowPos().x;
            vMax.y += ImGui::GetWindowPos().y;
            ImGui::Image((ImTextureID)resTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();
        }
        ImGui::End();
        if (helpFlag) {
            ImGui::SetNextWindowSize(ImVec2(350.0f, 100.0f), 0);
            ImGui::SetNextWindowPos(ImVec2(885.0f, 490.0f), 0);
            ImGui::Begin("Help");
            {
                ImGui::Text(helpString.c_str());
            }
            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (takeImage) takeSS(), takeImage = !takeImage;
        if (AAFlag != prevAAFlag) {
            if (AAFlag) changeSamples((GLsizei)8);
            else changeSamples((GLsizei)1);
            prevAAFlag = !prevAAFlag;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    baseShader.deleteProg();
    cleanUp();

    return 0;
}
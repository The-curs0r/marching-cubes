#include <iostream>
#include <vector>
#include <set>
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
#include <Windows.h>

#include "shader.hpp"
//#include "cube.hpp"
#include "chunk.hpp"
#include "control.hpp"
#include "vboIndexer.hpp"
#include "triangleFace.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//extern "C" {
//    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

GLFWwindow* window;
const int SCR_WIDTH = 1920;
const int SCR_HEIGHT = 1080;
const int MAX_THREADS = 4;

glm::mat3 mvMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat3 projMatrix = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);

GLuint vao, vbo, nbo, ebo;
GLuint vaot, vbot, nbot, ebot;
GLuint fbo, resTex, resDep;
GLuint mul_fbo, mul_resTex, mul_resDep;
ImVec2 vMin, vMax;
double xpos, ypos; //For mouse input

std::vector<glm::vec3> points;
std::vector<glm::vec3> quadVertices;
std::vector<glm::vec3> lines;

int takeImage = 0;
bool AAFlag = false, prevAAFlag = false;
std::string displayFormat = "%.3f";

GLuint noiseTex;

//Debug
float counter = 0;

//Cubes
//std::vector<cube> cubes;
std::vector<chunk> chunks;

//Tris
std::vector<glm::vec3> finalTris;
std::vector<glm::vec3> finalNorms;
std::vector<unsigned short> indices;///<Vector to store indicies of triangles to be plotted
std::vector<glm::vec3> indexed_vertices;///<Vector to stored indexed vertices
std::vector<glm::vec3> indexed_normals;///<Vector to stored indexed normals

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
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    return;
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
    //Borderless in release mode, don't fool around unless you want to sign out or restart
#if defined(_DEBUG)
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Template", NULL, NULL);
#else
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    window = glfwCreateWindow(mode->width, mode->height, "Template", primary, NULL);
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);//For Key Input
    glfwPollEvents();//Continously Checks For Input
    glfwSetCursorPos(window, 1920 / 2, 1080 / 2);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

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

    //Set quad vertices i.e. 2 triangles
    quadVertices.push_back(glm::vec3(-0.5f * 2.0f, 0.5f * 2.0f, 0.0f));
    quadVertices.push_back(glm::vec3(-0.5f * 2.0f, -0.5f * 2.0f, 0.0f));
    quadVertices.push_back(glm::vec3(0.5f * 2.0f, -0.5f * 2.0f, 0.0f));
    quadVertices.push_back(glm::vec3(0.5f * 2.0f, 0.5f * 2.0f, 0.0f));
    quadVertices.push_back(glm::vec3(-0.5f * 2.0f, 0.5f * 2.0f, 0.0f));
    quadVertices.push_back(glm::vec3(0.5f * 2.0f, -0.5f * 2.0f, 0.0f));

    //Load noise texture
    int w, h, comp;
    std::string filename = "./Textures/noise.png";
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(filename.c_str(), &w, &h, &comp, STBI_rgb);
    stbi_set_flip_vertically_on_load(0);
    if (image == nullptr) {
        throw(std::string("Failed to load snow texture"));
    }
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &noiseTex);
    glBindTexture(GL_TEXTURE_2D, noiseTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (comp == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else if (comp == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(image);

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

    //Converting to PNG
    WinExec("cd ..", 1);
    WinExec("magick \"./ScreenShot.tga\" -flip \"./Output.png\"", 1);

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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
            takeSS();
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

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glDeleteTextures(1, &mul_resTex);
    glDeleteTextures(1, &mul_resDep);
    glDeleteTextures(1, &resDep);
    glDeleteTextures(1, &resTex);
    points.clear();
    lines.clear();
    return;
}
void draw(Shader baseShader) {
    computeMatricesFromInputs(window);

    glBindTexture(GL_TEXTURE_2D, noiseTex);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    baseShader.setMat4("mv_matrix", ViewMatrix);
    baseShader.setMat4("proj_matrix", ProjectionMatrix);

    baseShader.setVec3("lightPos", getPosition());
    
    //glBindVertexArray(vao);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //if (finalTris.size()) {
    //    glBufferData(GL_ARRAY_BUFFER, finalTris.size() * sizeof(glm::vec3), &finalTris[0], GL_STATIC_DRAW);
    //    glDrawArrays(GL_TRIANGLES, 0, finalTris.size());
    //}
    
    glBindBuffer(GL_ARRAY_BUFFER, vbot);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    //glBindBuffer(GL_ARRAY_BUFFER, nbot);
    //glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebot);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
   

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)(0));

    return;
}

void calcTris() {
    if (chunks.size()) {
        int cnt = 0;
        for (auto i : chunks) {
            for (auto cube : i.cubes) {
                cnt++;
                std::vector<glm::vec3> tris = cube.triangles;
                //std::cout << tris.size() << "\n";
                if (tris.size()) {
                    finalTris.insert(finalTris.end(), tris.begin(), tris.end());
                }
            }
        }
        std::cout << cnt << "\n";
    }
    indexVBO(finalTris, indices, indexed_vertices);

    std::vector<triangleFace> faces;
    //std::vector<std::set<int> >* faceBelongTo = new std::vector<std::set<int> > (indices.size);
    for (int i = 0;i < indices.size();i+=3) {
        triangleFace temp = triangleFace(i / 3, indices[i], indices[i + 1], indices[i + 2], indexed_vertices);
        //faceBelongTo[indices[i]].insert(i / 3);
        //faceBelongTo[indices[i+1]].insert(i / 3);
        //faceBelongTo[indices[i+2]].insert(i / 3);
        faces.push_back(temp);
    }
    //std::vector<glm::vec3> indexed_normals;
    for (int i = 0;i < indexed_vertices.size();i++) {
        indexed_normals.push_back(glm::vec3(0.0f));
    }
    for (int i = 0;i < faces.size();i++) {
        indexed_normals[faces[i].indices[0]] += faces[i].normal;
        indexed_normals[faces[i].indices[1]] += faces[i].normal;
        indexed_normals[faces[i].indices[2]] += faces[i].normal;
    }
    for (int i = 0;i < indexed_vertices.size();i++) {
        indexed_normals[i] = glm::normalize(indexed_normals[i]);
    }

    //for (auto i : indices)
    //    std::cout << i <<"  "<<indexed_vertices[i].x << "  " << indexed_vertices[i].y<< "  " << indexed_vertices[i].z << "\n";

    glGenBuffers(1, &vbot);
    glBindBuffer(GL_ARRAY_BUFFER, vbot);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &nbot);
    glBindBuffer(GL_ARRAY_BUFFER, nbot);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
    std::cout << finalTris.size() << " " << finalTris.size() * sizeof(glm::vec3) << "\n";
    
    //delete[] faceBelongTo;
}


void test() {
    //std::vector<float> noise = {-1.0,.5f,-0.6f,-0.8f,-0.3f,-0.7f,1.4f,-0.6f};
    //std::vector<glm::vec3> vertexCoord;
    //vertexCoord.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
    //vertexCoord.push_back(glm::vec3(-1.0f, 1.0f, -1.0f));
    //vertexCoord.push_back(glm::vec3(1.0f, 1.0f, -1.0f));
    //vertexCoord.push_back(glm::vec3(1.0f,-1.0f,-1.0f));
    //vertexCoord.push_back(glm::vec3(-1.0f, -1.0f, 1.0f));
    //vertexCoord.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
    //vertexCoord.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    //vertexCoord.push_back(glm::vec3(1.0f, -1.0f, 1.0f));
    //cube newCube = cube(vertexCoord, noise);
    //newCube.generateTriangles();
    //cubes.push_back(newCube);
    int xrange = 5;
    int yrange = 5;
    int zrange = 5;
    const int numCubes = 16; //Change in compute shader too
    float length = 2.0f;

    for (int i = 0;i < xrange;i++) {
        for (int j = 0;j < yrange;j++) {
            for (int k = 0;k < zrange;k++) {
                
                //chunk newChunk = chunk(glm::vec3(i, j, k), numCubes, length);
                //newChunk.generateCubes();
                ////for (auto cube : newChunk.cubes) {
                //    //cube.generateTriangles();
                //    //std::cout << cube.triangles.size() << "\n";
                //    //;
                ////}
                //chunks.push_back(newChunk);

                Shader computeShader("cShader.computeShader.glsl");
                computeShader.use();
                computeShader.setFloat("inc", length / numCubes);
                computeShader.setInt("numCubes", numCubes);
                computeShader.setVec3("stPoint", glm::vec3(i, j, k));
                float* noiseVal = new float[numCubes * numCubes * numCubes * 8]; //Number of total vertices
                glm::vec3* vertexNormal = new glm::vec3[numCubes * numCubes * numCubes * 8];
                GLuint  data_buffer[1];
                glGenBuffers(1, data_buffer);
                int NUM_ELEMENTS = numCubes * numCubes * numCubes * 8;
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, data_buffer[0]);
                glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_ELEMENTS * sizeof(float), NULL, GL_DYNAMIC_COPY);

                glShaderStorageBlockBinding(computeShader.ID, 0, 0);

                glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, data_buffer[0], 0, sizeof(float) * NUM_ELEMENTS);
                computeShader.use();

                glDispatchCompute(numCubes, numCubes, numCubes);

                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
                glFinish();

                glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, data_buffer[0], 0, sizeof(float) * NUM_ELEMENTS);
                float* ptr = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * NUM_ELEMENTS, GL_MAP_READ_BIT);
                while (NUM_ELEMENTS--) {
                    /*float x = *ptr;
                    ptr++;
                    float y = *ptr;
                    ptr++;
                    float z = *ptr;
                    ptr++;
                    vertexNormal[numCubes * numCubes * numCubes * 8 - NUM_ELEMENTS - 1] = glm::vec3(x,y,z);*/
                    noiseVal[numCubes * numCubes * numCubes * 8 - NUM_ELEMENTS - 1] = *ptr;
                    ptr++;
                }
                glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
                
                chunk newChunk = chunk(glm::vec3(i, j, k), numCubes, length);
                newChunk.generateCubes(noiseVal);
                //newChunk.generateCubes(noiseVal, vertexNormal);
                ////for (auto cube : newChunk.cubes) {
                //    //cube.generateTriangles();
                //    //std::cout << cube.triangles.size() << "\n";
                //    //;
                ////}
                chunks.push_back(newChunk);
                delete[] noiseVal;
                delete[] vertexNormal;
            }
        }
    }
    calcTris();
    return;
}

void compute() {

    //const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
    //const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model

    //std::cout << vendor << "\n";
    //std::cout << renderer << "\n";


    //int NumberOfExtensions;
    //glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
    //std::cout << NumberOfExtensions << "\n";
    //for (int i = 0; i < NumberOfExtensions; i++) {
    //    const GLubyte* ccc = glGetStringi(GL_EXTENSIONS, i);
    //    std::cout << ccc << "\n";
    //    if (strcmp((const char*)ccc, (const char*)"GL_EXT_shader_16bit_storage") == 0) {
    //        // The extension is supported by our hardware and driver
    //        // Try to get the "glDebugMessageCallbackARB" function :
    //        std::cout << "yeye\n";
    //    }
    //}
    

    return;
}

int main() {

    if (initialize() < 0)
        return -1;

    test();

    Shader baseShader("vShader.vertexShader.glsl", "fShader.fragmentShader.glsl");
    baseShader.use();

    compute();
    

    baseShader.use();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {
        counter += 0.1;

        processInput(window);
        cursor_position_callback(window, xpos, ypos);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, mul_fbo);

        //Draw calls here
        draw(baseShader);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, mul_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        ImGui::SetNextWindowSize(ImVec2(1350.0f, 950.0f), 0);
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

        ImGui::Begin("Controls");
        {
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            if (ImGui::Button("Clear Points")) {
                ;
            }
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::Text("Utility");
            ImGui::Checkbox("Anti-aliasing", &AAFlag);
            ImGui::SameLine();
            ImGui::Dummy(ImVec2(10.0f, 0.0f));
            ImGui::SameLine();
            /*if (ImGui::Button("Take Screenshot")) {
                takeImage = 1;
            }*/
        }
        ImGui::End();
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
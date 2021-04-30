cmake_minimum_required(VERSION 3.1)
project(marchingCubes)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps")
set(SOURCES 
"${SRC_DIR}/Source.cpp" 
"${SRC_DIR}/control.cpp" 
"${SRC_DIR}/vboIndexer.cpp" 
"${LIB_DIR}/imgui/imgui.cpp" 
"${LIB_DIR}/imgui/imgui_demo.cpp" 
"${LIB_DIR}/imgui/imgui_draw.cpp" 
"${LIB_DIR}/imgui/imgui_tables.cpp" 
"${LIB_DIR}/imgui/imgui_widgets.cpp" 
"${LIB_DIR}/imgui/backends/imgui_impl_opengl3.cpp" 
"${LIB_DIR}/imgui/backends/imgui_impl_glfw.cpp" 
"${LIB_DIR}/glad/src/glad.c" 
)
if(WIN32)
	set(SOURCES ${SOURCES} "${LIB_DIR}/imgui/backends/imgui_impl_win32.cpp ")
endif()
set(HEADERS 
"${SRC_DIR}/control.hpp" 
"${SRC_DIR}/vboIndexer.hpp" 
"${SRC_DIR}/shader.hpp" 
"${SRC_DIR}/triangleFace.hpp" 
"${LIB_DIR}/imgui/imgui.h" 
"${LIB_DIR}/imgui/imconfig.h" 
"${LIB_DIR}/imgui/imgui_internal.h" 
"${LIB_DIR}/imgui/imstb_rectpack.h" 
"${LIB_DIR}/imgui/imstb_textedit.h" 
"${LIB_DIR}/imgui/imstb_truetype.h" 
"${LIB_DIR}/imgui/backends/imgui_impl_opengl3.h" 
"${LIB_DIR}/imgui/backends/imgui_impl_glfw.h" 
)
if(WIN32)
	set(SOURCES ${SOURCES} "${LIB_DIR}/imgui/backends/imgui_impl_win32.h ")
endif()
set(SHADERS 
"${SRC_DIR}/vShader.vertexShader.glsl" 
"${SRC_DIR}/vShaderFlat.vertexShader.glsl" 
"${SRC_DIR}/fShader.fragmentShader.glsl" 
"${SRC_DIR}/fShaderFlat.fragmentShader.glsl" 
"${SRC_DIR}/marchShader.computeShader.glsl" 
)
if(UNIX AND NOT APPLE)
	find_package(GLEW)
	if (GLEW_FOUND)
		message("GLEW already present")
		set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -g -ldl -lGL -lGLU -lGLEW")
	else()
		message("GLEW not present")
		set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -g -ldl")
	endif()
	add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
endif()

file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# glad
include_directories("${LIB_DIR}/imgui" "${LIB_DIR}/glm" "${LIB_DIR}/glad/include")

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${SHADERS})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")
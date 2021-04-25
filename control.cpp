/**
	* File:    control.cpp
	*
	* Author1:  Akshay Tiwari (2018A7PS0201H)
	* Author2:  Mushkan Surekha  (2018B5A70477H)
	* Author3:  Mahesh Swaminathan  (2018A4PS0982H)
	* Date:     09-04-2020
	* Course:   Computer Graphics (IS F311)
	*
	* Summary of File:
	*
	*   Program to control the movement of camera around the scene using
	*	either mouse or keyboard inputs as desired by the user.
*/
#include <GLFW/glfw3.h>
extern GLFWwindow* window; ///<Current context window
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include <iostream>
#include "control.hpp"

glm::mat4 ViewMatrix;///<Output View Matrix
glm::mat4 ProjectionMatrix;///<Output Projection Matrix
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);///<Initial camera location
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);///<Initial camera front location (view direction)
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);///<Initial camera up direction
glm::vec3 position = glm::vec3(0, 0, 2);///<Initial position on +Z
float horizontalAngle = 3.14f;///<Initial horizontal angle : toward -Z
float verticalAngle = 0.0f;///<Initial vertical angle : none
float initialFoV = 45.0f;///<Initial Field of View
float speed = 1.0f;///<Speed of viweing
float mouseSpeed = 0.001f;///<Cursor movement speed
float yaw = 0;///<Initial yaw
float pitch = 0;///<Initial pitch
float roll = 0;///<Initial roll
float FoV = initialFoV;///<Initial Field Of View
glm::vec3 center;///<Center vector
glm::vec3 up;//Up vector
glm::vec3 direction;//Direction
glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}
glm::vec3 getPosition() {
	return position;
}
void computeMatricesFromInputs(GLFWwindow* window) {
	static double lastTime = glfwGetTime();
	double xpos, ypos;
	double currentTime = glfwGetTime();
	// Compute time difference between current and last frame
	float deltaTime = float(currentTime - lastTime);
	// Get mouse position
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, 1920 / 2, 1080 / 2);
	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1920 / 2 - xpos);
	verticalAngle += mouseSpeed * float(1080 / 2 - ypos);
	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = glm::vec3(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
	direction = glm::normalize(direction);
	glm::vec3 right = glm::normalize(glm::vec3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f)));
	up = glm::normalize(glm::cross(right, direction));
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;

	}
	center = position + direction;
	ViewMatrix = glm::lookAt(position, position + direction, up);
	lastTime = currentTime;
	
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 16.0f / 9.0f, 0.1f, 100.0f);
}


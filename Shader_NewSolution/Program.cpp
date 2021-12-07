#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>
#include "loadShader.h"

//#ifdef WIN32
//This magic line is to force notebook computer that share NVidia and Intel graphics to use high performance GPU (NVidia).
//Intel graphics doesn't support OpenGL > 1.2
extern "C" _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
//#endif

/* Processes an OpenGL error message callback */
static void GLAPIENTRY GLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) 
{
	std::string warningMessage;
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		warningMessage = "LOG";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		warningMessage = "WARN";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		warningMessage = "ERROR";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		warningMessage = "FATAL";
		break;
	default:
		warningMessage = "UNKNOWN";
		break;
	}

	std::cout << "[OPENGL " << warningMessage << "](" << type << ") : " << message << std::endl;

	if (severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH)
	{
		std::cout << "Breaking execution because an error was raised above medium severity!" << std::endl;
		__debugbreak();
	}
}

struct vertex_struct {
	GLfloat positions[3];
	GLfloat color[2];
	GLfloat uv[2];
	GLfloat normal[3];
};


static void error_callback(int /*error*/, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}
static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	/* Create a windowed mode window and its OpenGL context */

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	//glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Something went wrong!" << std::endl;
		exit(-1);
	}

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLMessageCallback, 0);
	std::cout << "OpenGL debug enabled" << std::endl;

	GLfloat positions[] = { 
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, -1.0f, 1.0f
	};
	// Buffers
	GLuint vbo, vao, ebo;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	std::cout << sizeof(positions) << std::endl;
	
	glVertexAttribPointer(
		0,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		sizeof(positions)/(_countof(positions)/3),
		(GLvoid*)0					// array buffer offset
	);

	const auto programId = LoadShaders("../Ressources/shaders/shader.vert", "../Ressources/shaders/shader.frag");

	const auto index = glGetAttribLocation(programId, "aPos");

	/*glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(positions), nullptr);*/
	glEnableVertexAttribArray(index);

	// Callbacks
	//glDebugMessageCallback(opengl_error_callback, nullptr);
	glUseProgram(programId);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);

		//Loop body // Draw here
		glBindVertexArray(vao);		
		glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDisableVertexAttribArray(0);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
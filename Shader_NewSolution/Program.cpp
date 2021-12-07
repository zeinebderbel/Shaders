#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>
#include "loadShader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
	GLfloat color[3];
	GLfloat uv[2];
	GLfloat normal[3];
};

static const std::vector<vertex_struct> vertices{
	{{1.0f, -1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f,-1.0f}},
	{{1.0f, 1.0f, 1.0f},  {0.0f,1.0f,0.0f}, {1.0f,0.0f}, {0.0f,0.0f,-1.0f}},
	{{-1.0f, -1.0f, 1.0f}, {0.0f,0.0f,1.0f}, {0.0f,1.0f}, {0.0f,0.0f,-1.0f}},
    {{-1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f}, {0.0f,0.0f,-1.0f}}
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

	unsigned int indices[]
	{
		1,0,2,
		3,2,1
	};
	// Buffers
	GLuint vbo, vao, ebo;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_struct)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	std::cout << sizeof(vertex_struct) << std::endl;
	
	const auto programId = LoadShaders("../Ressources/shaders/shader.vert", "../Ressources/shaders/shader.frag");


	GLint positionLocation = glGetAttribLocation(programId, "vertexPosition_modelspace");
	glVertexAttribPointer(
		positionLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		sizeof(vertex_struct),					// stride
		(void*)offsetof(vertex_struct, vertex_struct::positions)			// array buffer offset
	);
	glEnableVertexAttribArray(positionLocation);

	//EnableAttrib<vertex_struct>("vertexPosition_modelspace", programID, 3);

	GLint colorLocation = glGetAttribLocation(programId, "color");
	glVertexAttribPointer(
		colorLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		sizeof(vertex_struct),					// stride
		(void*)offsetof(vertex_struct, vertex_struct::color)			// array buffer offset
	);
	glEnableVertexAttribArray(colorLocation);

	GLint uvLocation = glGetAttribLocation(programId, "uv");
	glVertexAttribPointer(
		uvLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
		2,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		sizeof(vertex_struct),					// stride
		(void*)offsetof(vertex_struct, vertex_struct::uv)			// array buffer offset
	);
	glEnableVertexAttribArray(uvLocation);

	GLint normalLocation = glGetAttribLocation(programId, "normal");
	glVertexAttribPointer(
		normalLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		sizeof(vertex_struct),					// stride
		(void*)offsetof(vertex_struct, vertex_struct::normal)			// array buffer offset
	);

	//texture 
	int width, height, nrChannels;
	unsigned char *data = stbi_load("..\Ressource\tex.jpg", &width, &height, &nrChannels, 0);
	unsigned int texture;
	glGenTextures(1, &texture);
	// Callbacks
	//glDebugMessageCallback(opengl_error_callback, nullptr);
	glUseProgram(programId);

	double t;

	GLuint scale_uniform_location = glGetUniformLocation(programId, "scale");
	GLuint scale_frag_uniform_location = glGetUniformLocation(programId, "scale_frag");
	GLuint width_uniform_location = glGetUniformLocation(programId, "width");
	GLuint height_uniform_location = glGetUniformLocation(programId, "height");
	GLuint mode_location = glGetUniformLocation(programId, "mode");
	int mode = 0;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);

		t = glfwGetTime();
		float scale_uniform = std::sin(t);

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			mode = 0;
		}
		else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			mode = 1;
		}
		else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			mode = 2;
		}
		else {
			mode = -1;
		}

		glUniform1f(scale_uniform_location, scale_uniform);
		glUniform1f(scale_frag_uniform_location, scale_uniform);
		glUniform1i(width_uniform_location, width);
		glUniform1i(height_uniform_location, height);
		glUniform1i(mode_location, mode);

		//Loop body // Draw here
		glBindVertexArray(vao);		
		//glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDisableVertexAttribArray(0);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
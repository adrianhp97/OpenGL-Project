#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "loadShader.cpp"
using namespace glm;

const int radius = 5;
GLfloat twicePi = 2.0f * M_PI;
/*
void drawFrontWheel() {
	static GLFloat frontWheel[];
	float x = 15.0;
	float y = 0;
	int triangleAmount = 20;
	for(int i = 0; i < triangleAmount*3; i++) {
		switch(i%3) {
			case 0:
				frontWheel[i] = (
									x + (radius * cos(i * twicePi / triangleAmount)),
								 	y + (radius * sin(i * twicePi / triangleAmount)), 0.0);
				break;
			case 1:
				frontWheel[i] = (x, y, 0.0);
				break;
			case 2:
				if(i == triangleAmount*3 - 1) {
					frontWheel[i] = (
										x + (radius * cos(0)),
									 	y + (radius * sin(0)), 0.0);
					
				} else {
					frontWheel[i] = (
									x + (radius * cos(i+1 * twicePi / triangleAmount)),
								 	y + (radius * sin(i+1 * twicePi / triangleAmount)), 0.0);
					
				} break;
		}
		
	}

}

void drawBackWheel() {
	static GLFLoat backWheel[];
}
*/
int main(){
	//credits to http://www.opengl-tutorial.org
	if( !glfwInit() ) {
	    fprintf( stderr, "Failed to initialize GLFW\n" );
	    return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow( 1024, 768, "Tugas 02 - Car Using Shaders", NULL, NULL);
	if( window == NULL ){
	    fprintf( stderr, "Failed to open GLFW window.");
	    glfwTerminate();
	    return -1;
	}
	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
	    fprintf(stderr, "Failed to initialize GLEW\n");
	    return -1;
	}


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	static GLfloat g_vertex_buffer_data[] = {
	   -19.0f, 0.0f, 0.0f,
	   -27.0f, 1.0f, 0.0f,
	   -27.0f, 10.0f, 0.0f,

	   -27.0f, 10.0f, 0.0f,
	   -19.0f, 0.0f, 0.0f,
	   -20.0f, 12.0f, 0.0f,

	   -20.0f, 12.0f, 0.0f,
	   -10.0f, 20.0f, 0.0f,
	   20.0f, 19.0f, 0.0f,

	   20.0f, 19.0f, 0.0f,
	   -20.0f, 12.0f, 0.0f,
	   23.0f, 15.0f, 0.0f,

	   23.0f, 15.0f, 0.0f,
	   -20.0f, 12.0f, 0.0f,
	   23.0f, 1.0f, 0.0f,

	   23.0f, 1.0f, 0.0f,
	   -20.0f, 12.0f, 0.0f,
	   22.0f, 0.0f, 0.0f,

	   22.0f, 0.0f, 0.0f,
	   -20.0f, 12.0f, 0.0f,
	   -19.0f, 0.0f, 0.0f
	};

	for(int i = 0; i < 7*3*3; i++) {
		g_vertex_buffer_data[i] /= 30.0;
	}

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint programID = LoadShaders("vertexShader.glsl", "fragmentShader.glsl");

	do{
	    
		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUseProgram(programID);

	    // 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
		   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		   3,                  // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   0,                  // stride
		   (void*)0            // array buffer offset
		);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 21); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);

	    // Swap buffers
	    glfwSwapBuffers(window);
	    glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0 );
}


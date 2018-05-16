#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <iostream>


#define MAX_PARTICLES 65000

int FindUnusedRainParticle();
int FindUnusedSmokeParticle();

void SortRainParticles();
void SortSmokeParticles();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

struct Particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a;
	float size, angle, weight;
	float life;
	float cameradistance;

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

Particle RainParticlesContainer[MAX_PARTICLES];
int LastUsedRainParticle = 0;

Particle SmokeParticlesContainer[MAX_PARTICLES];
int LastUsedSmokeParticle = 0;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
float _R = 1.0f;
float _G = 1.0f;
float _B = 1.0f; 

// Addition
float _smokeSpread = 1.5f;
float _rainSpread = 5.0f;
float _numRainParticlesFactor = 1.0f;
float smokeParticleFactor = 1.0f;
float rainAngle = 0.0f;
float rainLife = 0.2f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("basic_lighting.vs", "basic_lighting.fs");
    Shader lampShader("lamp.vs", "lamp.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
     // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float wheel_vertices[] = {
        // Back
        -0.25f, -0.25f, -0.1f,  0.0f, 0.0f,
         0.25f, -0.25f, -0.1f,  1.0f, 0.0f,
         0.25f,  0.25f, -0.1f,  1.0f, 1.0f,
         0.25f,  0.25f, -0.1f,  1.0f, 1.0f,
        -0.25f,  0.25f, -0.1f,  0.0f, 1.0f,
        -0.25f, -0.25f, -0.1f,  0.0f, 0.0f,

        // Front
        -0.25f, -0.25f,  0.1f,  0.0f, 0.0f,
         0.25f, -0.25f,  0.1f,  1.0f, 0.0f,
         0.25f,  0.25f,  0.1f,  1.0f, 1.0f,
         0.25f,  0.25f,  0.1f,  1.0f, 1.0f,
        -0.25f,  0.25f,  0.1f,  0.0f, 1.0f,
        -0.25f, -0.25f,  0.1f,  0.0f, 0.0f,

        // Left
        -0.25f,  0.25f,  0.1f,  1.0f, 0.0f,
        -0.25f,  0.25f, -0.1f,  1.0f, 1.0f,
        -0.25f, -0.25f, -0.1f,  0.0f, 1.0f,
        -0.25f, -0.25f, -0.1f,  0.0f, 1.0f,
        -0.25f, -0.25f,  0.1f,  0.0f, 0.0f,
        -0.25f,  0.25f,  0.1f,  1.0f, 0.0f,
        
        // Right
         0.25f,  0.25f,  0.1f,  1.0f, 0.0f,
         0.25f,  0.25f, -0.1f,  1.0f, 1.0f,
         0.25f, -0.25f, -0.1f,  0.0f, 1.0f,
         0.25f, -0.25f, -0.1f,  0.0f, 1.0f,
         0.25f, -0.25f,  0.1f,  0.0f, 0.0f,
         0.25f,  0.25f,  0.1f,  1.0f, 0.0f,

        // Bottom
        -0.25f, -0.25f, -0.1f,  0.0f, 1.0f,
         0.25f, -0.25f, -0.1f,  1.0f, 1.0f,
         0.25f, -0.25f,  0.1f,  1.0f, 0.0f,
         0.25f, -0.25f,  0.1f,  1.0f, 0.0f,
        -0.25f, -0.25f,  0.1f,  0.0f, 0.0f,
        -0.25f, -0.25f, -0.1f,  0.0f, 1.0f,

        // Top
        -0.25f,  0.25f, -0.1f,  0.0f, 1.0f,
         0.25f,  0.25f, -0.1f,  1.0f, 1.0f,
         0.25f,  0.25f,  0.1f,  1.0f, 0.0f,
         0.25f,  0.25f,  0.1f,  1.0f, 0.0f,
        -0.25f,  0.25f,  0.1f,  0.0f, 0.0f,
        -0.25f,  0.25f, -0.1f,  0.0f, 1.0f
    };

    int wheel_vertices_sizes = ((sizeof wheel_vertices) / (sizeof * wheel_vertices));

    float vertices[] = {
        // Back
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        // Front
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        // Left
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        // EXT LEFT
            // er front
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.0f, 0.5f, 1.0f, 0.5f,
        -1.0f, 0.0f, 0.5f, 1.5f, 0.5f,

        -0.5f, 0.0f, 0.5f,  1.0f, 0.5f,
        -1.0f, 0.0f, 0.5f,  1.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -1.0f, -0.5f, 0.5f, 1.5f, 0.0f,
        -1.0f, 0.0f, 0.5f,  1.5f, 0.5f,

        -1.0f, 0.0f, 0.5f,  1.5f, 0.5f,
        -1.5f, 0.0f,0.5f,   2.0f, 0.5f,
        -1.0f, -0.5f,0.5f,  1.5f, 0.0f,
        -1.0f, -0.5f, 0.5f, 1.5f, 0.0f,
        -1.5f, -0.5f, 0.5f, 2.0f, 0.0f,
        -1.5f, 0.0f,  0.5f, 2.0f, 0.5f,

        //er back
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.0f, -0.5f, 1.0f, 0.5f,
        -1.0f, 0.0f, -0.5f, 1.5f, 0.5f,

        -0.5f, 0.0f, -0.5f,  1.0f, 0.5f,
        -1.0f, 0.0f, -0.5f, 1.5f, 0.5f,
        -0.5f, -0.5f,-0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,-0.5f,  1.0f, 0.0f,
        -1.0f, -0.5f,-0.5f,  1.5f, 0.0f,
        -1.0f, 0.0f, -0.5f, 1.5f, 0.5f,

        -1.0f, 0.0f,  -0.5f,  1.5f, 0.5f,
        -1.5f, 0.0f,  -0.5f,   2.0f, 0.5f,
        -1.0f, -0.5f, -0.5f,  1.5f, 0.0f,
        -1.0f, -0.5f, -0.5f, 1.5f, 0.0f,
        -1.5f, -0.5f, -0.5f, 2.0f, 0.0f,
        -1.5f, 0.0f,  -0.5f, 2.0f, 0.5f,

        // er top
        -1.0f, 0.0f, 0.5f,  1.5f, 0.0f, 
        -1.5f, 0.0f, -0.5f, 2.0f, 1.0f,
        -1.0f, 0.0f, -0.5f, 1.5f, 1.0f, 
        -1.0f, 0.0f, 0.5f,  1.5f, 0.0f,
        -1.5f, 0.0f, -0.5f, 2.0f, 1.0f,
        -1.5f, 0.0f, 0.5f,  2.0f, 0.0f,
        
        // er right
        -1.5f, 0.0f, 0.5f,   0.5f, 0.0f,
        -1.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -1.5f, 0.0f, -0.5f,  0.5f, 1.0f,
        -1.5f, 0.0f, 0.5f,   0.5f, 0.0f,
        -1.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -1.5f, -0.5f, 0.5f,  1.0f, 0.0f,

        // er middle
        -0.5f, 0.5f, 0.5f,  1.0f, 0.0f,  
        -1.0f, 0.0, 0.5f, 1.0f, 0.0f,
        -1.0f, 0.0, -0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f,
        -1.0f, 0.0, -0.5f, 1.0f, 0.0f,
        
        // EXT RIGHT
            // er front
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 0.5f,
        1.0f, 0.0f, 0.5f, 1.5f, 0.5f,

        0.5f, 0.0f, 0.5f,  1.0f, 0.5f,
        1.0f, 0.0f, 0.5f,  1.5f, 0.5f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        1.0f, -0.5f, 0.5f, 1.5f, 0.0f,
        1.0f, 0.0f, 0.5f,  1.5f, 0.5f,

        1.0f, 0.0f, 0.5f,  1.5f, 0.5f,
        1.5f, 0.0f,0.5f,   2.0f, 0.5f,
        1.0f, -0.5f,0.5f,  1.5f, 0.0f,
        1.0f, -0.5f, 0.5f, 1.5f, 0.0f,
        1.5f, -0.5f, 0.5f, 2.0f, 0.0f,
        1.5f, 0.0f,  0.5f, 2.0f, 0.5f,

            //er back
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.0f, -0.5f, 1.0f, 0.5f,
        1.0f, 0.0f, -0.5f, 1.5f, 0.5f,

        0.5f, 0.0f, -0.5f,  1.0f, 0.5f,
        1.0f, 0.0f, -0.5f, 1.5f, 0.5f,
        0.5f, -0.5f,-0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,-0.5f,  1.0f, 0.0f,
        1.0f, -0.5f,-0.5f,  1.5f, 0.0f,
        1.0f, 0.0f, -0.5f, 1.5f, 0.5f,

        1.0f, 0.0f,  -0.5f,  1.5f, 0.5f,
        1.5f, 0.0f,  -0.5f,   2.0f, 0.5f,
        1.0f, -0.5f, -0.5f,  1.5f, 0.0f,
        1.0f, -0.5f, -0.5f, 1.5f, 0.0f,
        1.5f, -0.5f, -0.5f, 2.0f, 0.0f,
        1.5f, 0.0f,  -0.5f, 2.0f, 0.5f,

        // er top
        1.0f, 0.0f, 0.5f,  1.5f, 0.0f, 
        1.5f, 0.0f, -0.5f, 2.0f, 1.0f,
        1.0f, 0.0f, -0.5f, 1.5f, 1.0f, 
        1.0f, 0.0f, 0.5f,  1.5f, 0.0f,
        1.5f, 0.0f, -0.5f, 2.0f, 1.0f,
        1.5f, 0.0f, 0.5f,  2.0f, 0.0f,
        
        // er right
        1.5f, 0.0f, 0.5f,   0.5f, 0.0f,
        1.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        1.5f, 0.0f, -0.5f,  0.5f, 1.0f,
        1.5f, 0.0f, 0.5f,   0.5f, 0.0f,
        1.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        1.5f, -0.5f, 0.5f,  1.0f, 0.0f,

        // er middle
        0.5f, 0.5f, 0.5f,  0.0f, 0.0f,  
        1.0f, 0.0, 0.5f, 0.0f, 0.0f,
        1.0f, 0.0, -0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f,
        1.0f, 0.0, -0.5f, 0.0f, 0.0f,


        // Bottom
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        // Top
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    static GLfloat* rain_position = new GLfloat[MAX_PARTICLES * 4];
	static GLubyte* rain_color = new GLubyte[MAX_PARTICLES * 4];
	for (int i = 0; i < MAX_PARTICLES; i++) {
		RainParticlesContainer[i].life = -1.0f;
		RainParticlesContainer[i].cameradistance = -1.0f;
	}
	static const GLfloat rain_vertexes[] = {
		-0.01f, -0.1f, 0.0f,
		0.01f, -0.1f, 0.0f,
		-0.01f, 0.1f, 0.0f,
		0.01f, 0.1f, 0.0f,
	};

    static GLfloat* smoke_position = new GLfloat[MAX_PARTICLES * 4];
	static GLubyte* smoke_color = new GLubyte[MAX_PARTICLES * 4];
	for (int i = 0; i < MAX_PARTICLES; i++) {
		SmokeParticlesContainer[i].life = -1.0f;
		SmokeParticlesContainer[i].cameradistance = -1.0f;
	}
	static const GLfloat smoke_vertexes[] = {
		-0.1f, -0.1f, 0.0f,
		0.1f, -0.1f, 0.0f,
		-0.1f, 0.1f, 0.0f,
		0.1f, 0.1f, 0.0f,
	};

    int vertice_sizes = (sizeof(vertices)/sizeof(*vertices));

    // world space positions of car body
    glm::vec3 carBodyPositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
    };
    int car_body_number = (sizeof(carBodyPositions)/sizeof(*carBodyPositions));

    // world space positions of car wheel
    glm ::vec3 carWheelPositions[] = {
        glm::vec3(0.75f, -0.5f, 0.5f),
        glm::vec3(-0.75f, -0.5f, 0.5f),
        glm::vec3(0.75f, -0.5f, -0.5f),
        glm::vec3(-0.75f, -0.5f, -0.5f),
    };
    int car_wheel_number = (sizeof(carWheelPositions)/sizeof(*carWheelPositions));

    unsigned int VBO, VAO, wheelVBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &wheelVBO);

    // load and create a texture 
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("texture_wood.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    lightingShader.use();
    lightingShader.setInt("texture1", 0);
    lightingShader.setInt("texture2", 1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

        // Degree for wheel rotation
    int rotationAngle = 0;

	// Create Vertex Array Object
	GLuint RainVAO;
	glGenVertexArrays(1, &RainVAO);
	glBindVertexArray(RainVAO);
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint RainVBO;
	glGenBuffers(1, &RainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, RainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rain_vertexes), rain_vertexes, GL_STATIC_DRAW);
	GLuint RainPositionVBO;
	glGenBuffers(1, &RainPositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, RainPositionVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	GLuint RainColorVBO;
	glGenBuffers(1, &RainColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, RainColorVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    Shader RainShader("RainVertexShader.vs", "RainFragmentShader.fs");

    RainShader.use();
    RainShader.setVec3("RainCameraRight",0,0,0);
    RainShader.setVec3("RainCameraUp",0,0,0);
    RainShader.setMat4("RainVP", camera.GetViewMatrix());
	RainShader.setInt("rainTextureSampler", 0);

    // Create Vertex Array Object
	GLuint SmokeVAO;
	glGenVertexArrays(1, &SmokeVAO);
	glBindVertexArray(SmokeVAO);
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint SmokeVBO;
	glGenBuffers(1, &SmokeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SmokeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smoke_vertexes), smoke_vertexes, GL_STATIC_DRAW);
	GLuint SmokePositionVBO;
	glGenBuffers(1, &SmokePositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SmokePositionVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	GLuint SmokeColorVBO;
	glGenBuffers(1, &SmokeColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SmokeColorVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    Shader SmokeShader("SmokeVertexShader.vs", "SmokeFragmentShader.fs");

    SmokeShader.use();
    SmokeShader.setVec3("SmokeCameraRight",0,0,0);
    SmokeShader.setVec3("SmokeCameraUp",0,0,0);
    SmokeShader.setMat4("SmokeVP", camera.GetViewMatrix());
	SmokeShader.setInt("smokeTextureSampler", 0);

    double lastTime = glfwGetTime();
    int nframes = 0;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        double currentTime = glfwGetTime();
        nframes++;
        if (currentTime -lastTime >= 1.0) {
            printf("%.0f FPS\n", double(nframes));
            nframes = 0;
            lastTime += 1.0;
        }
        // input
        processInput(window);

       // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // change the light's position values over time (can be done anywhere in the render loop actually, but try to do it at least before using the light source positions)
        lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();

        lightingShader.setInt("texture1", 0);
        lightingShader.setInt("texture2", 1);

        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", _R, _G, _B);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // render car body
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);

        for (unsigned int i = 0; i < car_body_number; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate(model, carBodyPositions[i]);
            float angle = /*2*/0.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, vertice_sizes);
        }

        // // render car wheel
        // glBindBuffer(GL_ARRAY_BUFFER, wheelVBO);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(wheel_vertices), wheel_vertices, GL_STATIC_DRAW);
        // // position attribute
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        // glEnableVertexAttribArray(0);
        // glEnableVertexAttribArray(1);
        // // texture coord attribute
        // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        // glEnableVertexAttribArray(2);

        // Add rotation
        rotationAngle = (rotationAngle+1) % 360;
        
        for (unsigned int i = 0; i < car_wheel_number; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate(model, carWheelPositions[i]);
            float angle = /*2*/0.0f * i;
            model = glm::rotate(model, (float)rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
            lightingShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, wheel_vertices_sizes);
        }

        // also draw the lamp object
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        glm::mat4 model;
        model = glm::mat4();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lampShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        /* RAIN */
		glm::mat4 RainProjectionMatrix = projection;
		glm::mat4 RainViewMatrix = view;
		glm::vec3 RainCameraPosition(glm::inverse(RainViewMatrix)[3]);
		glm::mat4 RainViewProjectionMatrix = RainProjectionMatrix * RainViewMatrix;
        glm::mat4 prevRainViewProjectionMatrix = RainViewProjectionMatrix;
		// Generate 10 new particule each millisecond but limit to 60 fps
		int rainNewparticles = (int)(deltaTime*10000.0*_numRainParticlesFactor);
		// if (rainNewparticles > (int)(0.016f*10000.0)) {
		// 	rainNewparticles = (int)(0.016f*10000.0);
		// }
		for (int i = 0; i < rainNewparticles; i++) {
			int rainParticleIndex = FindUnusedRainParticle();
			RainParticlesContainer[rainParticleIndex].life = rainLife;
			RainParticlesContainer[rainParticleIndex].pos = glm::vec3(
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f
			);
			float rainSpread = _rainSpread;
			glm::vec3 rainMaindir = glm::vec3(0.0f, -10.0f, 0.0f);
			// Random direction
			glm::vec3 rainRandomdir = glm::vec3(
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f
			);
			RainParticlesContainer[rainParticleIndex].speed = rainMaindir + rainRandomdir * rainSpread;
            // Rotation angle
            RainViewProjectionMatrix = prevRainViewProjectionMatrix;
            RainViewProjectionMatrix = glm::rotate(RainViewProjectionMatrix, (float) rainAngle, glm::vec3(0.0f, 0.0f, 1.0f));
			// Random color
			RainParticlesContainer[rainParticleIndex].r = 255;//rand() % 256;
			RainParticlesContainer[rainParticleIndex].g = 255;//rand() % 256;
			RainParticlesContainer[rainParticleIndex].b = 255;//rand() % 256;
			RainParticlesContainer[rainParticleIndex].a = (rand() % 256) / 3;
			RainParticlesContainer[rainParticleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;
		}
		// Simulate all particles
		int rainParticlesCount = 0;
		for (int i = 0; i < MAX_PARTICLES; i++) {
			Particle& p = RainParticlesContainer[i];
			if (p.life > 0.0f) {
				// Decrease life
				p.life -= deltaTime;
				if (p.life > 0.0f) {
					// Simulate simple physics : gravity only, no collisions
					p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)deltaTime * 0.5f;
					p.pos += p.speed * (float)deltaTime;
					p.cameradistance = glm::length2(p.pos - RainCameraPosition);
					// Fill the GPU buffer
					rain_position[4 * rainParticlesCount + 0] = p.pos.x;
					rain_position[4 * rainParticlesCount + 1] = p.pos.y;
					rain_position[4 * rainParticlesCount + 2] = p.pos.z;
					rain_position[4 * rainParticlesCount + 3] = p.size;
					rain_color[4 * rainParticlesCount + 0] = p.r;
					rain_color[4 * rainParticlesCount + 1] = p.g;
					rain_color[4 * rainParticlesCount + 2] = p.b;
					rain_color[4 * rainParticlesCount + 3] = p.a;
				}
				else {
					// Particles that just died will be put at the end of the buffer in SortParticles()
					p.cameradistance = -1.0f;
				}
				rainParticlesCount++;
			}
		}
		SortRainParticles();
		// Use our shader
		RainShader.use();
		// Bind our texture in Texture Unit 0
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, RainTexture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		// glUniform1i(RainTextureID, 0);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		RainShader.setVec3("RainCameraRight", RainViewMatrix[0][0], RainViewMatrix[1][0], RainViewMatrix[2][0]);
		RainShader.setVec3("RainCameraUp", RainViewMatrix[0][1], RainViewMatrix[1][1], RainViewMatrix[2][1]);
		RainShader.setMat4("RainVP", RainViewProjectionMatrix);
		// Draw object
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindBuffer(GL_ARRAY_BUFFER, RainPositionVBO);
		glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, rainParticlesCount * sizeof(GLfloat) * 4, rain_position);
		glBindBuffer(GL_ARRAY_BUFFER, RainColorVBO);
		glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, rainParticlesCount * sizeof(GLubyte) * 4, rain_color);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, RainVBO);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// Position object
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, RainPositionVBO);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		// Color object
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, RainColorVBO);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, rainParticlesCount);


        /* SMOKE */
		// Setup camera matrix
		glm::mat4 SmokeProjectionMatrix = projection;
		glm::mat4 SmokeViewMatrix = view;
		glm::vec3 SmokeCameraPosition(glm::inverse(SmokeViewMatrix)[3]);
		glm::mat4 SmokeViewProjectionMatrix = SmokeProjectionMatrix * SmokeViewMatrix;
		// Generate 10 new particule each millisecond but limit to 60 fps
		int smokeNewparticles = (int)(deltaTime*10000.0*smokeParticleFactor);
		// if (smokeNewparticles > (int)(0.016f*10000.0)) {
		// 	smokeNewparticles = (int)(0.016f*10000.0);
		// }
		for (int i = 0; i < smokeNewparticles; i++) {
			int smokeParticleIndex = FindUnusedSmokeParticle();
			SmokeParticlesContainer[smokeParticleIndex].life = 0.2f;
			SmokeParticlesContainer[smokeParticleIndex].pos = glm::vec3(-0.9f, -0.4f, 0.0f);
			float smokeSpread = _smokeSpread;
			glm::vec3 smokeMaindir = glm::vec3(-10.0f, 0.0f, 0.0f);
			// Random direction
			glm::vec3 smokeRandomdir = glm::vec3(
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f
			);
			SmokeParticlesContainer[smokeParticleIndex].speed = smokeMaindir + smokeRandomdir * smokeSpread;
			// Random color
			// SmokeParticlesContainer[smokeParticleIndex].r = rand() % 256;
			// SmokeParticlesContainer[smokeParticleIndex].g = rand() % 256;
			// SmokeParticlesContainer[smokeParticleIndex].b = rand() % 256;
             SmokeParticlesContainer[smokeParticleIndex].r = 3;
			SmokeParticlesContainer[smokeParticleIndex].g = 3;
			SmokeParticlesContainer[smokeParticleIndex].b = 3;
			SmokeParticlesContainer[smokeParticleIndex].a = (rand() % 256) / 3;
			SmokeParticlesContainer[smokeParticleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;
		}
		// Simulate all particles
		int smokeParticlesCount = 0;
		for (int i = 0; i < MAX_PARTICLES; i++) {
			Particle& p = SmokeParticlesContainer[i];
			if (p.life > 0.0f) {
				// Decrease life
				p.life -= deltaTime;
				if (p.life > 0.0f) {
					// Simulate simple physics : gravity only, no collisions
					p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)deltaTime * 0.5f;
					p.pos += p.speed * (float)deltaTime;
					p.cameradistance = glm::length2(p.pos - SmokeCameraPosition);
					// Fill the GPU buffer
					smoke_position[4 * smokeParticlesCount + 0] = p.pos.x;
					smoke_position[4 * smokeParticlesCount + 1] = p.pos.y;
					smoke_position[4 * smokeParticlesCount + 2] = p.pos.z;
					smoke_position[4 * smokeParticlesCount + 3] = p.size;
					smoke_color[4 * smokeParticlesCount + 0] = p.r;
					smoke_color[4 * smokeParticlesCount + 1] = p.g;
					smoke_color[4 * smokeParticlesCount + 2] = p.b;
					smoke_color[4 * smokeParticlesCount + 3] = p.a;
				}
				else {
					// Particles that just died will be put at the end of the buffer in SortParticles()
					p.cameradistance = -1.0f;
				}
				smokeParticlesCount++;
			}
		}
		SortSmokeParticles();
		// Use our shader
		SmokeShader.use();
		// Bind our texture in Texture Unit 0
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, SmokeTexture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		// glUniform1i(SmokeTextureID, 0);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		SmokeShader.setVec3("SmokeCameraRight", SmokeViewMatrix[0][0], SmokeViewMatrix[1][0], SmokeViewMatrix[2][0]);
		SmokeShader.setVec3("SmokeCameraUp", SmokeViewMatrix[0][1], SmokeViewMatrix[1][1], SmokeViewMatrix[2][1]);
		SmokeShader.setMat4("SmokeVP", SmokeViewProjectionMatrix);
		// Draw object
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindBuffer(GL_ARRAY_BUFFER, SmokePositionVBO);
		glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, smokeParticlesCount * sizeof(GLfloat) * 4, smoke_position);
		glBindBuffer(GL_ARRAY_BUFFER, SmokeColorVBO);
		glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, smokeParticlesCount * sizeof(GLubyte) * 4, smoke_color);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, SmokeVBO);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// Position object
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, SmokePositionVBO);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		// Color object
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, SmokeColorVBO);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, smokeParticlesCount);



        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &SmokeVBO);
	glDeleteBuffers(1, &SmokePositionVBO);
	glDeleteBuffers(1, &SmokeColorVBO);
	glDeleteBuffers(1, &RainVBO);
	glDeleteBuffers(1, &RainPositionVBO);
	glDeleteBuffers(1, &RainColorVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        _R += 0.1f; _G += 0.1f; _B += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        _R -= 0.1f; _G -= 0.1f; _B -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        _smokeSpread -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        _smokeSpread += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        _rainSpread -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        _rainSpread += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        _numRainParticlesFactor -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        _numRainParticlesFactor += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (rainAngle < 0.0f) rainAngle += 0.1f;
        rainLife -= 0.005f;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (rainAngle > -0.75f) rainAngle -= 0.1f;
        rainLife += 0.005f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) {
        smokeParticleFactor -= 0.1f;    
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        smokeParticleFactor += 0.1f;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

int FindUnusedRainParticle() {
	for (int i = LastUsedRainParticle; i < MAX_PARTICLES; i++) {
		if (RainParticlesContainer[i].life < 0) {
			LastUsedRainParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedRainParticle; i++) {
		if (RainParticlesContainer[i].life < 0) {
			LastUsedRainParticle = i;
			return i;
		}
	}

	return 0;
}

void SortRainParticles() {
	std::sort(&RainParticlesContainer[0], &RainParticlesContainer[MAX_PARTICLES]);
}

int FindUnusedSmokeParticle() {
	for (int i = LastUsedSmokeParticle; i < MAX_PARTICLES; i++) {
		if (SmokeParticlesContainer[i].life < 0) {
			LastUsedSmokeParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedSmokeParticle; i++) {
		if (SmokeParticlesContainer[i].life < 0) {
			LastUsedSmokeParticle = i;
			return i;
		}
	}

	return 0;
}

void SortSmokeParticles() {
	std::sort(&SmokeParticlesContainer[0], &SmokeParticlesContainer[MAX_PARTICLES]);
}

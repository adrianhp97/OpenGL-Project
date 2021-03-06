#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

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
        glm::mat4 view = camera.GetViewMatrix();
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


        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

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
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

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
    Shader ourShader("texture.vs", "texture.fs");

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
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &wheelVBO);
    // glBindBuffer(GL_ARRAY_BUFFER, wheelVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(wheel_vertices), wheel_vertices, GL_STATIC_DRAW);

    // position attribute
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // // texture coord attribute
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);


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
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    // Degree for wheel rotation
    int rotationAngle = 0;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
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

        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);

        // render car body
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        for (unsigned int i = 0; i < car_body_number; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate(model, carBodyPositions[i]);
            float angle = /*2*/0.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, vertice_sizes);
        }

        // render car wheel
        glBindBuffer(GL_ARRAY_BUFFER, wheelVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(wheel_vertices), wheel_vertices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Add rotation
        rotationAngle = (rotationAngle+1) % 360;
        for (unsigned int i = 0; i < car_wheel_number; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate(model, carWheelPositions[i]);
            float angle = /*2*/0.0f * i;
            model = glm::rotate(model, (float)rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, wheel_vertices_sizes);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
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

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
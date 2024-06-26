#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <utility>
#include "shader.h"
#include "object.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture.h"
#include <fstream>

int SRC_WIDTH = 1920;
int SRC_HEIGHT = 1080;
const int X_AMOUNT = 1920 / 5;
const int Y_AMOUNT = 1080 / 10;
const float COUNT = 384 / 2;
float DELTA_L = SRC_WIDTH / COUNT;

#include "camera.h"

float zoomLevel = 1.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::ortho((float)-SRC_WIDTH / 2, (float)SRC_WIDTH / 2, (float)-SRC_HEIGHT / 2, (float)SRC_HEIGHT / 2, -1.0f, 1.0f)); // Global Camera for the entire code thing :)
bool isDragging = false;
double lastX, lastY;
bool debug = false; // Turning this to true enables the grid and other debug messaging
bool autoFollow = false;

#include "compute.h"
#include "particle.h"
#include <time.h>
#include "rotator.h" // Requires camera to be initialized for it to work
#include "fourier.h" 
#include "plotter.h" 

std::string HOME_DIRECTORY = "/home/hiatus/Documents/FourierCircleDrawing";

// Whenever the window is changed this function is called
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// Mouse call back
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// Processing all input here
void processInput(GLFWwindow *window);
// Scrol callback
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
// Mouse callback
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);


// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

Plotter *plotter; 
NestedCircles *fourierDrawer; 
Object *fourierPathDrawer; 

int main()
{
    // ------------ OPENGL INTIALIZATION ----------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);                 // The major version so x.x the first x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // The minor version of GLFW we are using so x.x the second x
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // This means we do not use any backwards compatible features, so its a smaller set of all of OPENGL

    // Creating the window object
    GLFWwindow *window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "Fourier OpenGL Demo 1.0", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Load GLAD function pointers so that we use the correct openGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // OPENGL INITIALIZATION
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0f);

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Important vectors to track
    std::vector<float> positions;
    std::vector<float> outputPositions;

    for (int i = 0; i < X_AMOUNT; i++)
    {
        for (int j = 0; j < Y_AMOUNT; j++)
        {
            positions.push_back(i * DELTA_L - SRC_WIDTH / 2);
            positions.push_back(j * DELTA_L - SRC_HEIGHT / 2);
        }
    }
    Shader normalGlobalShader(std::string(HOME_DIRECTORY + std::string("/src/shaders/regularVert.vs")).c_str(), std::string(HOME_DIRECTORY + std::string("/src/shaders/frag.fs")).c_str());
    Shader modifiedGridShader(std::string(HOME_DIRECTORY + std::string("/src/shaders/modifiedVert.vs")).c_str(), std::string(HOME_DIRECTORY + std::string("/src/shaders/frag.fs")).c_str());


    // GRID SPACING DEBUG / DEMO CODE
    std::vector<float> grid;
    for (int i = -SRC_WIDTH / (2); i < SRC_WIDTH / 2; i += DELTA_L)
    {
        grid.push_back(i + DELTA_L / 2);
        grid.push_back(-1080.0f / 2);
        grid.push_back(i + DELTA_L / 2);
        grid.push_back(1080.0f / 2);
    }
    for (int i = -SRC_HEIGHT / (2); i < SRC_HEIGHT / 2; i += DELTA_L)
    {
        grid.push_back(-SRC_WIDTH / 2);
        grid.push_back(i + DELTA_L / 2);
        grid.push_back(SRC_WIDTH / 2);
        grid.push_back(i + DELTA_L / 2);
    }

    Object gridObject(&modifiedGridShader, grid, {1.0f, 1.0f, 1.0f, 1.0f});

    Rotator rotator(0, 50, 100, 100, 1); 

    fourierPathDrawer = new Object(&normalGlobalShader); 

    fourierDrawer = new NestedCircles(); 
    plotter = new Plotter(); // Handles mouse input for drawing 

    // Main Loop of the function
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // std::cout << "FPS: " << 1 / deltaTime << std::endl;

        // Clear the screen before we start
        glClearColor(24 / 255.0f, 24 / 255.0f, 24 / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.projection = glm::ortho((float)-SRC_WIDTH / (2 * zoomLevel), (float)SRC_WIDTH / (2 * zoomLevel), (float)-SRC_HEIGHT / (2 * zoomLevel), (float)SRC_HEIGHT / (2 * zoomLevel), -1.0f, 1.0f);

        // Process input call
        processInput(window);

        // Get mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Convert from screen space to NDC
        xpos = xpos / SRC_WIDTH * 2.0 - 1.0;
        ypos = 1.0 - ypos / SRC_HEIGHT * 2.0;

        // Adjust for camera position and zoom
        xpos = xpos * (SRC_WIDTH / (2.0 * zoomLevel)) + camera.position.x;
        ypos = ypos * (SRC_HEIGHT / (2.0 * zoomLevel)) + camera.position.y;

        int timeMod = 100; 

        fourierDrawer->render(glfwGetTime()/(float)10); 
        
        plotter->renderPoints(); 
        
        if (fourierDrawer->calculated == true){
            fourierPathDrawer->vertices.push_back(fourierDrawer->tipX); 
            fourierPathDrawer->vertices.push_back(fourierDrawer->tipY);
        }

        if (autoFollow){
            camera.position.x = fourierDrawer->tipX; 
            camera.position.y = fourierDrawer->tipY; 
        }

        fourierPathDrawer->render(camera.getViewMatrix(), camera.getProjectionMatrix(), GL_POINTS); 
        
        if (((int)glfwGetTime())%timeMod == 0 && glfwGetTime() - (int)glfwGetTime() < 0.01){
            fourierPathDrawer->vertices.clear(); 
        }
 
        // gridObject.render(camera.getViewMatrix(), camera.getProjectionMatrix(), GL_LINES);

        glfwSwapBuffers(window); // Swaps the color buffer that is used to render to during this render iteration and show it ot the output screen
        glfwPollEvents();        // Checks if any events are triggered, updates the window state andcalls the corresponding functions
    }

    glfwTerminate();
    return 0;
}

// This function is going to be used to resize the viewport everytime it is resized by the user
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    SRC_HEIGHT = height;
    SRC_WIDTH = width;
    glViewport(0, 0, width, height);
}

// Zooming callback
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    // Calculate new zoom level
    double newZoomLevel = zoomLevel * std::exp(-yoffset / 10.0);

    // Clamp the zoom level to a range
    double minVal = 0.01;
    double maxVal = 1000.0;
    newZoomLevel = std::max(minVal, newZoomLevel);
    newZoomLevel = std::min(maxVal, newZoomLevel);

    // Set the new zoom level
    if (autoFollow)
    {
        zoomLevel = newZoomLevel;
        DELTA_L = SRC_WIDTH / (COUNT * zoomLevel);
    }
    else
    {
        zoomLevel = newZoomLevel;
        DELTA_L = SRC_WIDTH / COUNT;
    }
}

// Mouse button callback
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        isDragging = true;
        glfwGetCursorPos(window, &lastX, &lastY);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        isDragging = false;
    }
    plotter->handleMouseInput(window, button, action, mods); 
}

// Mouse movement callback
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (isDragging && !autoFollow)
    {
        // Calculate the mouse's offset since the last frame
        double dx = xpos - lastX;
        double dy = ypos - lastY;

        // Update the camera's position
        camera.position.x -= dx * 1 / zoomLevel;
        camera.position.y += dy * 1 / zoomLevel;

        // Update the last mouse position
        lastX = xpos;
        lastY = ypos;
    }
    plotter->handleMouseMovement(window, xpos, ypos); 
}

/**
 * @brief Handles all user input given the window object, currently handles player movement
 *
 * @param window
 */
bool debugKeyPressed = false;
bool autoFollowKeyPressed = false;
void processInput(GLFWwindow *window)
{
    // // Function is used as follows player.processKeyboard(ENUM, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && debugKeyPressed == false)
    {
        debug = !debug;
        debugKeyPressed = true;
        // Now we draw circles
        fourierPathDrawer->vertices.clear(); 
        fourierDrawer->initializeCoefficients(computeFourierCoefficients(plotter->path, 21)); 
        
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
    {
        debugKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && autoFollowKeyPressed == false)
    {
        autoFollow = !autoFollow;
        autoFollowKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
    {
        autoFollowKeyPressed = false;
    }

}

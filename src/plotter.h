#ifndef PLOTTER_H
#define PLOTTER_H 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>  

enum state {
    DRAW, 
    REST
};

class Plotter{
public: 
    state plotterState = REST; 
    std::vector<float> path; 
    std::pair<double, double> lastMousePosition; 
    std::pair<double, double> currentMousePosition; 
    std::pair<int, int> mouseState; // First is current, second is previous mouse state, 1 is clicked, 0 is not clicked 
    Object *renderer; 
    Shader *plotterShader; 

    Plotter(){
        plotterShader = new Shader("/home/hiatus/Documents/FourierCircleDrawing/src/shaders/regularVert.vs", "/home/hiatus/Documents/FourierCircleDrawing/src/shaders/frag.fs"); 
        renderer = new Object(plotterShader);  
    }; 

    /**
     * @brief This function is run each frame to update based on the mouse information
     */
    void updatePoints(){
        // Ermm idk what tod o here lol 
    }

    std::pair<float, float>convertMouseCoordinateToScreenCoordinate(std::pair<double, double> position){
        std::pair<float, float> output; 
        output.first = (position.first)/SRC_WIDTH * 2.0 - 1.0; 
        output.second = (position.second)/SRC_HEIGHT * 2.0 - 1.0; 
        output.first = output.first * (SRC_WIDTH / (2.0 * zoomLevel)) + camera.position.x;  
        output.second= -output.second * (SRC_HEIGHT / (2.0 * zoomLevel)) + camera.position.y; 
        return output; 
    }

    void handleMouseInput(GLFWwindow *window, int button, int action, int mods){ 
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            mouseState.first = 1; 
            plotterState = DRAW; 
        }   

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
            mouseState.first = 0; 
            plotterState = REST; 
        }

        if (mouseState.second == 0 && mouseState.first == 1){
            path.clear(); 
        }

        mouseState.second = mouseState.second;      
    }

    void handleMouseMovement(GLFWwindow *window, double xPosition, double yPosition){ 
        currentMousePosition.first = xPosition; 
        currentMousePosition.second = yPosition; 

        std::pair<float, float> convertedCoordinates = convertMouseCoordinateToScreenCoordinate(currentMousePosition); 

        // Do mouse stuff here
        if (plotterState == DRAW && (lastMousePosition.first != currentMousePosition.first || lastMousePosition.second != currentMousePosition.second)){
            path.push_back(convertedCoordinates.first); 
            path.push_back(convertedCoordinates.second); 
        }

        lastMousePosition.first = currentMousePosition.first;
        lastMousePosition.second = currentMousePosition.second;  
    }

    void handleKeyboardInput(GLFWwindow *window){
        // Nothing yet 
    }

    // To be done in the future!
    void getPointsFromSVG(){ 

    }

    std::vector<float> getPath(){
        return path; 
    }

    void renderPoints(){
        renderer->vertices = path; 
        renderer->render(camera.getViewMatrix(), camera.getProjectionMatrix(), GL_POINTS); 
    }

}; 


#endif
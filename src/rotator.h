#ifndef ROTATOR_H
#define ROTATOR_H 

#include "shader.h"
#include "object.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// This is the amount of line segments to draw one sphere 
#define SPHERE_LINE_COUNT 100

class Rotator { 
public: 
    float x; 
    float y; 
    float deltaX; 
    float deltaY; // We will use these values to calculate the initial angle and magnitude 
    float magnitude; 
    float period; // We will use this to move it faster or slower 
    float initialAngle; 

    std::vector<float> positions; 
    
    Shader* rotatorShader; 
    Object* rotatorObject; 

    Rotator(float X, float Y, float dx, float dy, float Period){
        x = X; y = Y; deltaX = dx; deltaY = dy; period = Period; // Y value is the imaginary part, x is the real part 
        rotatorShader = new Shader("/home/hiatus/Documents/FourierCircleDrawing/src/shaders/regularVert.vs", "/home/hiatus/Documents/FourierCircleDrawing/src/shaders/frag.fs"); 
        rotatorObject = new Object(rotatorShader);
        magnitude = sqrt(dx * dx + dy * dy); 
        initialAngle = atan2(dy, dx); 
    }

    /**
     * @brief Get the Rotator Tip vector 
     * 
     * @param t time, ranges from 0 to 1  
     * @return glm::vec2 
     */
    glm::vec2 getRotatorTip(float t){
        return  glm::vec2(magnitude * cosf(t * 2 * M_PI * period + initialAngle) + x, magnitude * sinf(t * 2 * M_PI * period + initialAngle) + y);
    }

    /**
     * @brief Rotates a vector by a specified angle, created using a 2x2 rotation matrix 
     * 
     * @param vec 
     * @param angle 
     * @return glm::vec2 
     */
    glm::vec2 rotateVector(glm::vec2 vec, float angle){
        float xComponent = vec.x * cosf(angle) - vec.y * sinf(angle); 
        float yComponent = vec.x * sinf(angle) + vec.y * cosf(angle); 
        return glm::vec2(xComponent, yComponent); 
    }

    /**
     * @brief Renders the rotator object using a renderable object from the object.h header. Has to regenerate points each time and pass all that data, you could use instanced rendering to fix this, LATER YOU ISSUE :)
     * 
     * @param t 
     */
    void render(float t){
        positions.clear(); 
        // Start by generating the poitns of a circle
        for(int i = 0; i < SPHERE_LINE_COUNT; i++){
            float angle = (i / (float)SPHERE_LINE_COUNT) * 2 * M_PI; 
            positions.push_back(magnitude * cosf(angle) + x); 
            positions.push_back(magnitude * sinf(angle) + y);  
            float angle2 = ((i+1) / (float)SPHERE_LINE_COUNT) * 2 * M_PI; 
            positions.push_back(magnitude * cosf(angle2) + x); 
            positions.push_back(magnitude * sinf(angle2) + y); 
        }  
        
        
        /* Drawing the arrow for each of the circles, rotating a vector as wel*/  

        glm::vec2 deltaVec = getRotatorTip(t);  
        float scaler = 7.0f; 
        positions.push_back(deltaVec.x); 
        positions.push_back(deltaVec.y); 
        positions.push_back(x); 
        positions.push_back(y); 

        positions.push_back(rotateVector(-deltaVec + glm::vec2(x, y), - M_PI/4).x/scaler + deltaVec.x); 
        positions.push_back(rotateVector(-deltaVec + glm::vec2(x, y), - M_PI/4).y/scaler + deltaVec.y); 
        positions.push_back(deltaVec.x); 
        positions.push_back(deltaVec.y); 
        
        positions.push_back(rotateVector(-deltaVec+ glm::vec2(x, y), M_PI/4).x/scaler + deltaVec.x); 
        positions.push_back(rotateVector(-deltaVec+ glm::vec2(x, y), M_PI/4).y/scaler + deltaVec.y); 
        positions.push_back(deltaVec.x); 
        positions.push_back(deltaVec.y); 


        rotatorObject->vertices = positions;  
        rotatorObject->render(camera.getViewMatrix(), camera.getProjectionMatrix(), GL_LINES); 
    }

    
}; 

#endif 
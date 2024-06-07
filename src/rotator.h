#ifndef ROTATOR_H
#define ROTATOR_H 

#include "shader.h"
#include "object.h"
#include <glm>; 

// This is the amount of line segments to draw one sphere 
#define SPHERE_LINE_COUNT 100

class rotator { 
public: 
    float x; 
    float y; 
    float deltaX; 
    float deltaY; // We will use these values to calculate the initial angle and magnitude 
    float magnitude; 
    float period; // We will use this to move it faster or slower 
    

    rotator(float X, float Y, float dx, float dy, float Period){
        x = X; y = Y; deltaX = dx; deltaY = dy; period = Period; // Y value is the imaginary part, x is the real part 
    }

    void render(

    
}; 

#endif 
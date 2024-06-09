#ifndef FOURIER
#define FOURIER 

#include <glad/glad.h>
#include "shader.h" 
#include <vector>
#include <iostream> 

/**
 * @brief Returns the list of complex coefficients for the fourier transform
 * 
 * @param path 
 * @param fourierDepth 
 * @return std::vector<std::pair<float, float>> 
 */
std::vector<std::pair<float, float>> computeFourierCoefficients(std::vector<float> path, int fourierDepth){
    std::cout << "Coefficient computer called!" << std::endl; 
    std::vector<std::pair<float, float>> coefficients; 
    if (path.size()/2 < fourierDepth){
        throw std::invalid_argument("Path length is less than fourier depth, invalid"); 
    } 
    // We will start by computing the -nth coefficient for the fourier all the way to the 0th, to the nth 
    for(int i = 0; i <= fourierDepth; i++){
        int fourierIndex = i - (fourierDepth/2);
        float dt = 2 / (float)path.size(); 
        float realSum = 0.0f; 
        float imaginarySum = 0.0f; 

        // Integrate for one coefficient 
        for(int pathIndex = 0; pathIndex < path.size()/2; pathIndex+=2){
            float t = dt * ((float)pathIndex); 
            float realComponent = path[pathIndex]; 
            float imaginaryComponent = path[pathIndex+1]; 

            realSum += (realComponent * cosf (-fourierIndex * 2 * M_PI * t) + imaginaryComponent * sinf(-fourierIndex * 2 * M_PI * t)) * dt; 
            imaginarySum += (-realComponent * sinf (-fourierIndex * 2 * M_PI * t) + imaginaryComponent * cosf(-fourierIndex * 2 * M_PI * t)) * dt; 
        }

        coefficients.push_back({realSum, imaginarySum}); 
    }
    return coefficients; 
}

class NestedCircles {
public:  
    float tipX; 
    float tipY; 
    bool calculated = false; 
    std::vector<Rotator> rotators; 
    
    NestedCircles(){
        // Nothing yet 
    }

    void initializeCoefficients(std::vector<std::pair<float, float>> coefficients){
        // Now we need to fill up the circles vector based on these coefficients, they are nested on top of each other
        rotators.clear(); 
        float lastX = 0.0f; float lastY = 0.0f; 
        for(int i = 0; i < coefficients.size(); i++){
            Rotator tempRotator = Rotator(lastX, lastY, coefficients[i].first, coefficients[i].second, i - ((int)coefficients.size()/2) - 1); 
            rotators.push_back(tempRotator);     
            lastX = coefficients[i].first; 
            lastY = coefficients[i].second; 
        }
        calculated = true; 
    }

    void render(float t){ 
        float lastX = 0; 
        float lastY = 0; 
        for(int i = 0; i < rotators.size(); i++){
            rotators[i].x = lastX; 
            rotators[i].y = lastY; 
            rotators[i].render(t); 
            lastX = rotators[i].getRotatorTip(t).x; 
            lastY = rotators[i].getRotatorTip(t).y;  
            if (i == rotators.size()-1){
                tipX = lastX; 
                tipY = lastY; 
            }
        }
    }
};

#endif 
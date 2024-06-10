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

    std::vector<std::pair<float, float>> function;
    for(int i = 0; i < path.size(); i+=2){
        function.push_back({path[i], path[i+1]}); 
    }

    // We will start by computing the -nth coefficient for the fourier all the way to the 0th, to the nth 
    for(int i = 0; i <= fourierDepth; i++){
        float fourierIndex = i - (fourierDepth/2);
        float dt = 1 / (float)function.size(); 
        float realSum = 0.0f; 
        float imaginarySum = 0.0f; 

        for(int pathIndex = 0; pathIndex < function.size(); pathIndex+=1){
            float t = dt * ((float)pathIndex); 
            float realComponent = function[pathIndex].first; 
            float imaginaryComponent = function[pathIndex].second; 

            realSum += (realComponent * cosf (-fourierIndex * t * 2 * M_PI) + imaginaryComponent * sinf(-fourierIndex * t * 2 * M_PI)) * dt; 
            imaginarySum += (-realComponent * sinf (-fourierIndex * t * 2 * M_PI) + imaginaryComponent * cosf(-fourierIndex * t * 2 * M_PI)) * dt; 
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
        for(int i = 0; i < coefficients.size(); i++){
            float temp = i - (float)(coefficients.size()/2) + 1;
            Rotator tempRotator = Rotator(0.0f, 0.0f, coefficients[i].first, coefficients[i].second, (i - (float)(coefficients.size()/2) + 1)); 
            rotators.push_back(tempRotator);     
            
        }
        calculated = true; 
    }

    void render(float t){ 
        if (rotators.size() == 0){
            return; 
        }
        float lastX = 0.0f; 
        float lastY = 0.0f; 
        for(int i = 0; i < rotators.size(); i++){
            rotators[i].x = lastX; 
            rotators[i].y = lastY; 
            rotators[i].render(t); 
            if (i == rotators.size()-1){
                tipX = lastX; 
                tipY = lastY; 
            }
            lastX = rotators[i].getRotatorTip(t).x; 
            lastY = rotators[i].getRotatorTip(t).y;  

        }
    }
};

#endif 
// -------------------------------------------------
//
//  MIT License
//
//  Specular.frag
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/16/26.
//
// -------------------------------------------------

// Recreated this specular shader using the phong reflection method
// https://en.wikipedia.org/wiki/Phong_reflection_model 

#version 450
#pragma shader_stage( fragment )

layout (location = 0) in vec3 uLightDirection;
layout (location = 1) in vec3 uNormalVector;
layout (location = 2) in vec3 uAmbientColorInt;  // Intensity
layout (location = 3) in vec3 uDiffuseColorInt;  // Intensity
layout (location = 4) in vec3 uSpecularInt;      // Intensity
layout (location = 5) in vec3 uViewDirection;
layout (location = 6) in float uShinyInt;        // Intensity

layout (location = 0) out vec3 vColor;

void main()
{
    // Normalize the vectors to ensure it's correct
    vec3 N = normalize(uNormalVector);
    vec3 L = normalize(uLightDirection);
    vec3 V = normalize(uViewDirection);

    // We're going to use the Halfway Vector Calculation 
    // used by the Binn-Phong method 
    // It supposely looks better quality
    // Which we care about (performance + quality)
    vec3 H = normalize(L + V); 

    // Calculate the diffuse and specular components 
    float DiffuseDot = max(dot(L, N), 0.0);
    float SpecularDot = max(dot(N, H), 0.0);
    float SpecularFactor = pow(SpecularDot, uShinyInt);

    vec3 vDiffuse = uDiffuseColorInt * DiffuseDot;
    vec3 vSpecular = uSpecularInt * SpecularFactor;
    

    vColor = uAmbientColorInt + vDiffuse + vSpecular;
}


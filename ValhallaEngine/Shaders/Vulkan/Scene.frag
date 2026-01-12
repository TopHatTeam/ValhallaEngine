// -------------------------------------------------
//
//  MIT License
//
//  Scene.frag
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/4/26.
//
// -------------------------------------------------

#version 450
#pragma shader_stage( fragment )

struct LightData
{
    vec3    LightPosition;
    vec3    LightColor;
    float   LightIntensity;
    float   SpecularPower;
};

layout(set = 0, binding = 0) uniform SceneData
{
    vec3    CameraPosition;
    vec3    AmbientColor;
    float   AmbientIntensity;
    uint    LightCount;
} SceneData;

layout(set = 0, binding = 1) uniform Lights
{
    LightData lights[];
} Lights;

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 WorldPosition;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 vColor;

void main()
{
    vec4 CompleteColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 uNormal = normalize(normal);
    
    CompleteColor = SceneData.AmbientColor * SceneData.AmbientIntensity;
    
    for (uint i = 0u; i < SceneData.LightCount; i++)
    {
        vec3 uLight = normalize(lights[i].LightPosition - WorldPosition);
        
        float diffuse = max(dot(uNormal, uLight), 0.0);
        
        // The viewer vector (aka your eyes or the camera's position)
        vec3 viewer = normalize(SceneData.CameraPosition - WorldPosition);
        
        // The reflection light ray vector
        vec3 ray = reflect(-uLight, uNormal);
        
        // Compute how shinny something is
        float SpecularPower = pow(max(dot(ray, viewer), 0.0), lights[i].SpecularPower);
        CompleteColor += diffuse * lights[i].LightColor * lights[i].LightIntensity + SpecularPower * lights[i].LightColor;
    }
    
    vColor = vec4(CompleteColor, 1.0);
}

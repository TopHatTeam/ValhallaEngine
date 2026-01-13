// -------------------------------------------------
//
//  MIT License
//
//  Renderer.metal
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/2/26.
//
// -------------------------------------------------

#include <metal_stdlib>
using namespace metal;

#import "ValhallaShaderTypes.h"

typedef struct
{
    float4 position [[position]];
    
    /// This is used for lighting and other cool effects
    float3 WorldPosition;
    
    float3 normal;
} PixelData;

vertex PixelData ValhallaSceneVS(uint VertexID [[vertex_id]], constant VertexData* VertexInfo [[buffer(0)]], constant SceneData* SceneInfo [[buffer(1)]])
{
    PixelData out;
    
    /// 1. First we get the vertex position
    float4 position = float4(VertexInfo[VertexID].position.xy, 0.0, 1.0);
    
    /// 2. Second we transform by object matrix (Object Space -> World Space)
    float4 ScenePosition = SceneInfo->ObjectMatrix * position;
    out.WorldPosition = ScenePosition.xyz;
    
    /// 3. Third we transform by camera/view matrix (World -> Camera)
    float4 ViewPosition = SceneInfo->CameraMatrix * ScenePosition;
    
    /// 4. Fourth we transform by projection matrix
    out.position = SceneInfo->CameraMatrix * ViewPosition;
    
    /// 5. Then we transform normals the object's space to world space
    out.normal = normalize((SceneInfo->ObjectMatrix * float4(VertexInfo[VertexID].normal, 0.0)).xyz);
    
    return out;
}

fragment float4 ValhallaScenePS(PixelData in [[stage_in]], constant SceneLightingData* scene [[buffer(0)]], constant LightData* lights [[buffer(1)]] )
{
    float3 normal = normalize(in.normal);
    
    //float3 uLight = normalize(light->LightPosition - in.WorldPosition);
    
    /// Lambert diffuse method
    //float diffuse = max(dot(normal, uLight), 0.0);
    
    /// Bui Tuong Phong specular method
    //float3 viewer   = normalize(light->CameraPosition - in.WorldPosition); // The viewer vector (aka your eyes or the camera's view)
    //float3 ray      = reflect(-uLight, normal); // The reflection light ray vector
    
    //float SpecularPower = pow(max(dot(ray, viewer), 0.0), light->SpecularPower);
    
    float3 CompleteColor = scene->AmbientColor * scene->AmbientIntensity;
    
    for (uint i = 0; i < scene->LightCount; i++)
    {
        float3  uLight = normalize(lights[i].LightPosition - in.WorldPosition);
        float diffuse = max(dot(normal, uLight), 0.0);
        
        // The viewer vector (aka your eyes or the camera's view)
        float3 viewer = normalize(scene->CameraPosition - in.WorldPosition);
        
        // The reflection light ray vector
        float3 ray = reflect(-uLight, normal);
        
        // Compute how shinny something is
        float SpecularPower = pow(max(dot(ray, viewer), 0.0), lights[i].SpecularPower);
        CompleteColor += diffuse * lights[i].LightColor * lights[i].LightIntensity + SpecularPower * lights[i].LightColor;
    }
    
    return float4(CompleteColor, 1.0);
}

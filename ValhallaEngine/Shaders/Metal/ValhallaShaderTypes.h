// -------------------------------------------------
//
//  MIT License
//
//  ValhallaShaderTypes.h
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/2/26.
//
// -------------------------------------------------

#ifndef ValhallaShaderTypes_h
#define ValhallaShaderTypes_h

#import <simd/simd.h>

typedef enum ShaderBufferIndex
{
    SBI_VertexData      = 0,
    SBI_ViewportSize    = 1,
    SBI_Uniforms        = 2
} ShaderBufferIndex;

typedef struct
{
    simd_float3 position;
    simd_float3 normal;
} VertexData;

typedef struct
{
    /// This gives us the place where an 3D object is in a 3D space to place into the 3D scene
    simd_float4x4 ObjectMatrix;
    
    /// This gives us the place where the Camera (A.K.A View) is in a 3D space to display the current view in the 3D scene
    simd_float4x4 CameraMatrix;
    
    /// This gives us the Projection for displaying the 3D scene in perspective projection or other projections
    simd_float4x4 ProjectionMatrix;
    
    /// This helps us with lighting by givings us the camera's position
    simd_float3 CameraPosition;
} SceneData;

typedef struct
{
    simd_float3  ObjectMatrix;
    simd_float3  CameraPosition;
    uint         LightCount;
    
    /// The color of the ambient lighting
    simd_float3 AmbientColor;
    
    /// How intense is the ambient lighting
    float AmbientIntensity;
} SceneLightingData;

typedef struct
{
    simd_float3  LightPosition;
    simd_float3  LightColor;
    float        LightIntensity;
    
    /// It shows how shinny something is
    float SpecularPower;
} LightData;

#endif /* ValhallaShaderTypes_h */

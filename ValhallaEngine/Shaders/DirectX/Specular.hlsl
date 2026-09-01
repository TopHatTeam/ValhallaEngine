// -------------------------------------------------
//
//  MIT License
//
//  Specular.hlsl
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/16/26.
//
// -------------------------------------------------

// Recreated this specular shader using the phong reflection method
// https://en.wikipedia.org/wiki/Phong_reflection_model 

struct vOutput 
{
    float3 vColor : SV_Target0;
};

cbuffer MaterialParams : register(b0)
{
    float3 uAmbientColorInt;  // Intensity
    float3 uDiffuseColorInt;  // Intensity
    float3 uSpecularInt;      // Intensity
    float uShinyInt;          // Intensity
};

struct uInput 
{
    float3 uLightDirection : TEXCOORD0;
    float3 uNormalVector   : TEXCOORD1;
    float3 uViewDirection  : TEXCOORD2;
};

vOutput main(uInput input)
{
    vOutput output; 

    // Normalize the vectors to ensure the calculations are correct
    float3 N = normalize(input.uNormalVector);
    float3 L = normalize(input.uLightDirection);
    float3 V = normalize(input.uViewDirection);
    float3 H = normalize(L + V);

    // Calculate the diffuse and specular components 
    float DiffuseDot = max(dot(L, N), 0.0);
    float SpecularDot = max(dot(N, H), 0.0);
    float SpecularFactor = pow(SpecularDot, uShinyInt);

    float3 vDiffuse = uDiffuseColorInt * DiffuseDot;
    float3 vSpecular = uSpecularInt * SpecularFactor;

    output.vColor = uAmbientColorInt + vDiffuse + vSpecular;
    return output;
}

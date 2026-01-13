// -------------------------------------------------
//
//  MIT License
//
//  Halo.metal
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/1/26.
//
// -------------------------------------------------

#include <metal_stdlib>
using namespace metal;

struct OutVertex
{
    float4 position [[position]];
    float4 UVAndScreenPos;
};

float2 FisheyeUV(float2 uv, float compression, float zoom)
{
    float2 NegativePosUV = (2.0 * uv - 1.0);
    
    float scale = compression * atan(1.0 / compression);
    float RadiusDistance = length(NegativePosUV) * scale;
    float RadiusDirection = compression * tan(RadiusDistance / compression) * zoom;
    float phi = atan2(NegativePosUV.y, NegativePosUV.x);
    
    float2 NewUV = float2(RadiusDirection * cos(phi) + 1.0, RadiusDirection * sin(phi) + 1.0) / 2.0;
    
    return NewUV;
}

static inline float DiscMask(float2 uv)
{
    float2 center = float2(0.5, 0.5);
    float r = distance(uv, center);
    return smoothstep(1.0, 0.95, 1.0 - r);
}

fragment float3 ValhallaHaloPS(OutVertex in [[stage_in]],
                               texture2d<float> InputTexture [[texture(0)]],
                               sampler InputSampler [[sampler(0)]],
                               constant float& width [[buffer(0)]],
                               constant float& mask [[buffer(1)]],
                               constant float& compression [[buffer(2)]],
                               constant float& intensity [[buffer(3)]],
                               constant float& ChromaShift [[buffer(4)]])
{
    float2 CenterPoint = float2(0.5, 0.5);
    
    // --- UVs ---
    float2 uv       = in.UVAndScreenPos.xy;
    float2 FishUV   = FisheyeUV(uv, compression, 1.0);
    
    // --- Halo Distortion Vector ---
    float2 HaloVector = normalize(CenterPoint - uv) * width;
    
    // --- Halo Mask ---
    float HaloMask  = distance(uv, CenterPoint);
    HaloMask        = saturate(HaloMask * 2.0);
    HaloMask        = smoothstep(mask, 1.0, HaloMask);
    
    // --- Screen Border Mask ---
    float2 ScreenPos        = in.UVAndScreenPos.zw;
    float  ScreenBorderMask = DiscMask(ScreenPos) * DiscMask(ScreenPos * 0.8);
    ScreenBorderMask        = ScreenBorderMask * 0.95 + 0.05;
    
    // --- Chromatic Offsets ---
    float2 UVr = (FishUV - CenterPoint) * (1.0 + ChromaShift) + CenterPoint + HaloVector;
    float2 UVg = FishUV + HaloVector;
    float2 UVb = (FishUV - CenterPoint) * (1.0 - ChromaShift) + CenterPoint + HaloVector;
    
    // --- Sample Texture ---
    float3 OutColor;
    OutColor.r = InputTexture.sample(InputSampler, UVr).r;
    OutColor.g = InputTexture.sample(InputSampler, UVg).g;
    OutColor.b = InputTexture.sample(InputSampler, UVb).b;
    
    // --- Apply Masks and Intensity ---
    OutColor *= ScreenBorderMask * HaloMask * intensity;
    
    return OutColor;
}


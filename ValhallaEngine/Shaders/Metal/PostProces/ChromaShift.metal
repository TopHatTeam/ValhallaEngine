// -------------------------------------------------
//
//  MIT License
//
//  ChromaShift.metal
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
    float2 uv;
};

fragment float3 ValhallaChromaPS(OutVertex in [[stage_in]],
                                 texture2d<float> InputTexture [[texture(0)]],
                                 sampler InputSampler [[sampler(0)]],
                                 constant float& ChromaShift [[buffer(0)]])
{
    float2 uv = in.uv;
    float2 center = float2(0.5, 0.5);
    
    // --- Chromatic Offsets ---
    float2 UVr = (uv - center) * (1.0 + ChromaShift) + center;
    float2 UVb = (uv - center) * (1.0 - ChromaShift) + center;
    
    float r = InputTexture.sample(InputSampler, UVr).r;
    float g = InputTexture.sample(InputSampler, uv).g;
    float b = InputTexture.sample(InputSampler, UVb).b;
    
    return float3(r, g, b);
}

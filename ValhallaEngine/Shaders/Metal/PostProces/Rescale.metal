// -------------------------------------------------
//
//  MIT License
//
//  Rescale.metal
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

fragment float4 ValhallaRescalePS(OutVertex in [[stage_in]],
                                  texture2d<float> InputTexture [[texture(0)]],
                                  sampler InputSampler [[sampler(0)]],
                                  constant float2& viewportsize [[buffer(0)]])
{
    float2 uv = in.uv * viewportsize;
    float3 color = InputTexture.sample(InputSampler, uv).rgb;
    
    return float4(color, 0.0);
}



// -------------------------------------------------
//
//  MIT License
//
//  DualKawaseBlur.metal
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

fragment float4 ValhallaKawaseBlurDownsamplePS(OutVertex in [[stage_in]],
                                               texture2d<float> InputTexture [[texture(0)]],
                                               sampler InputSampler [[sampler(0)]],
                                               constant float2& BufferSize [[buffer(0)]])
{
    float2 uv = in.uv;
    float2 HalfPixel = (1.0 / BufferSize) * 0.5;
    
    // -- Offsets --
    float2 offsets[4] = {
        float2(-HalfPixel.x,  HalfPixel.y), /* TOP LEFT  */
        float2( HalfPixel.x,  HalfPixel.y), /* TOP RIGHT */
        float2( HalfPixel.x, -HalfPixel.y), /* BOTTOM RIGHT */
        float2(-HalfPixel.x, -HalfPixel.y), /* BOTTOM LEFT  */
    };
    
    float3 color = InputTexture.sample(InputSampler, uv).rgb * 4.0;
    for (int i = 0; i < 4; i++)
    {
        color += InputTexture.sample(InputSampler, uv + offsets[i]).rgb;
    }
    
    return float4(color / 8.0, 0.0);
}

fragment float4 ValhallaKawaseBlurUpsamplePS(OutVertex in [[stage_in]],
                                               texture2d<float> InputTexture [[texture(0)]],
                                               sampler InputSampler [[sampler(0)]],
                                               constant float2& BufferSize [[buffer(0)]])
{
    float2 uv = in.uv;
    float2 HalfPixel = (1.0 / BufferSize) * 0.5;
    
    // --- Diagonal Offsets ---
    float2 diag[4] = {
        float2(-HalfPixel.x,  HalfPixel.y), /* TOP LEFT  */
        float2( HalfPixel.x,  HalfPixel.y), /* TOP RIGHT */
        float2( HalfPixel.x, -HalfPixel.y), /* BOTTOM RIGHT */
        float2(-HalfPixel.x, -HalfPixel.y)  /* BOTTOM LEFT  */
    };
    
    // --- Axis Offsets ---
    float2 axis[4] = {
        float2(-HalfPixel.x, 0.0), /* LEFT   */
        float2( HalfPixel.x, 0.0), /* RIGHT  */
        float2(0.0,  HalfPixel.y), /* TOP    */
        float2(0.0, -HalfPixel.y)  /* BOTTOM */
    };
    
    float3 color = float3(0.0);
    
    // --- Sample Diagonals ---
    for (int i = 0; i < 4; i++)
    {
        color += InputTexture.sample(InputSampler, uv + diag[i]).rgb;
    }
    
    // --- Sample Axes with Weight Z ---
    for (int i = 0; i < 4; i++)
    {
        color += InputTexture.sample(InputSampler, uv + axis[i]).rgb * 2.0;
    }
    
    return float4(color / 12.0, 0.0);
}

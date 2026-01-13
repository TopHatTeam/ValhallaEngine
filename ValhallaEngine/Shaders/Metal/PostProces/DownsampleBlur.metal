// -------------------------------------------------
//
//  MIT License
//
//  DownsampleBlur.metal
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

fragment float3 ValhallaDownsampleThresholdPS(OutVertex in [[stage_in]],
                                              texture2d<float> InputTexture [[texture(0)]],
                                              sampler InputSampler [[sampler(0)]],
                                              constant float2& InputSize [[buffer(0)]],
                                              constant float& ThresholdLevel [[buffer(1)]],
                                              constant float& ThresholdRange [[buffer(2)]])
{
    float2 uv = in.uv;
    float2 InPixelSize = 1.0 / InputSize;
    
    float3 OutColor = float3(0.0);
    
    // --- 4 Central Samples ---
    float2 CenterUVs[4] = {
        uv + InPixelSize * float2(-1.0,  1.0),
        uv + InPixelSize * float2( 1.0,  1.0),
        uv + InPixelSize * float2(-1.0, -1.0),
        uv + InPixelSize * float2( 1.0, -1.0)
    };
    
    for (int i = 0; i < 4; i++)
    {
        OutColor += InputTexture.sample(InputSampler, CenterUVs[i]).rgb;
    }
    
    OutColor = (OutColor / 4.0) * 0.5;
    
    // --- Threshold ---
    float luminance = dot(OutColor, float3(1.0));
    float ThresholdScale = saturate((luminance - ThresholdLevel) / ThresholdRange);
    OutColor *= ThresholdScale;
    
    return OutColor;
}

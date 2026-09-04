// -------------------------------------------------
//
//  MIT License
//
//  Ghosts.metal
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

static inline float DiscMask(float2 uv)
{
    float2 center = float2(0.5, 0.5);
    float r = distance(uv, center);
    return smoothstep(1.0, 0.95, 1.0 - r);
}

fragment float4 ValhallaGhostsPS(OutVertex in [[stage_in]],
                                 texture2d<float> InputTexture [[texture(0)]],
                                 sampler InputSampler [[sampler(0)]],
                                 constant float4* GhostColors [[buffer(0)]],
                                 constant float* GhostScales [[buffer(1)]],
                                 constant float& Intensity [[buffer(2)]])
{
    float2 uv = in.UVAndScreenPos.xy;
    float3 color = float3(0.0);
    
    // --- Loop, Calculate, and Display 8 Ghosts ---
    for (int i = 0; i < 8; i++)
    {
        float a = GhostColors[i].a * GhostScales[i];
        // --- Near to Zero ---
        if (abs(a) > 0.0001)
        {
            float2 NewUV = (uv - 0.5) * GhostScales[i];
            
            // --- Local Masks ---
            float DistanceMask  = 1.0 - length(NewUV);
            float mask          = smoothstep(0.5, 0.9, DistanceMask);
            float mask2         = smoothstep(0.75, 1.0, DistanceMask) * 0.95 + 0.05;
            
            // --- Sample and Accumulate ---
            color += InputTexture.sample(InputSampler, NewUV + 0.5).rgb * GhostColors[i].rgb * a * mask * mask2;
        }
    }
    
    // --- Screen Border Mask ---
    float2 ScreenPos = in.UVAndScreenPos.zw;
    float ScreenBorderMask = DiscMask(ScreenPos * 0.9);
    
    float3 OutColor = color * ScreenBorderMask * Intensity;
    return float4(OutColor, 0.0);
}

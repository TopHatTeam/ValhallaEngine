// -------------------------------------------------
//
//  MIT License
//
//  Mix.metal
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

struct LensFlareMixParams
{
    float   Intensity;
    float3  Tint;
    float2  BufferSize;
    float2  PixelSize;
    int3    MixPass;
};

fragment float4 ValhallaMixPS(OutVertex in [[stage_in]],
                       texture2d<float> InputTexture [[texture(0)]],
                       texture2d<float> BloomTexture [[texture(1)]],
                       texture2d<float> GlareTexture [[texture(2)]],
                       texture2d<float> GradientTexture [[texture(3)]],
                       sampler InputSampler [[sampler(0)]],
                       sampler GradientSampler [[sampler(1)]],
                       constant LensFlareMixParams& params [[buffer(0)]])
{
    float2 uv = in.uv;
    float3 OutColor = float3(0.0);
    
    // --- Add Bloom ---
    if (params.MixPass.x != 0)
    {
        OutColor += BloomTexture.sample(InputSampler, uv * params.BufferSize).rgb;
    }
    
    // --- Add Flare/Glare ---
    float3 flares = float3(0.0);
    
    if (params.MixPass.y != 0)
    {
        flares += InputTexture.sample(InputSampler, uv).rgb;
    }
    
    if (params.MixPass.z != 0)
    {
        constexpr float2 offsets[4] = {
            float2(-1.0,  1.0),
            float2( 1.0,  1.0),
            float2(-1.0, -1.0),
            float2( 1.0, -1.0)
        };
        
        float3 GlareColor = float(0.0);
        for (int i = 0; i < 4; i++)
        {
            float2 OffsetUV = uv + params.PixelSize * offsets[i];
            GlareColor += 0.25 * GlareTexture.sample(InputSampler, OffsetUV).rgb;
        }
        
        flares += GlareColor;
    }
    
    // --- Gradient Mask ---
    float2 center       = float2(0.5, 0.5);
    float2 GradientUV   = float2(saturate(distance(uv, center) * 2.0), 0.0);
    float3 gradient     = GradientTexture.sample(GradientSampler, GradientUV).rgb;
    
    // --- Final Mix ---
    OutColor += flares * gradient * params.Tint * params.Intensity;
    
    return float4(OutColor, 0.0);
}

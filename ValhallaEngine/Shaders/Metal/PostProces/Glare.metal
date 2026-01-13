// -------------------------------------------------
//
//  MIT License
//
//  Glare.metal
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/1/26.
//
// -------------------------------------------------

#include <metal_stdlib>
using namespace metal;

struct GlareVertex
{
    float2  position     [[attribute(0)]];
    float3  color        [[attribute(1)]];
    float   luminance    [[attribute(2)]];
    uint    id           [[attribute(3)]];
    uint    QuadIndex    [[attribute(4)]];
    uint    QuadSet      [[attribute(5)]];
};

vertex float4 ValhallaGlareVS(GlareVertex in [[stage_in]],
                              constant float2& BufferSize [[buffer(0)]],
                              constant float& GlareIntensity [[buffer(1)]],
                              constant float3* GlareScales [[buffer(2)]],
                              constant float4& GlareTint [[buffer(3)]],
                              constant float2& BufferRatio [[buffer(4)]])
{
    float2 TilePosition = in.position;
    float LuminanceScale = saturate(in.luminance / 1.0);
    float mask = 1.0 - saturate(length(TilePosition / BufferSize * 2.0 - 0.5) * 2.0);
    mask = mask * 0.6 + 0.4;
    
    float2 scale = float2(LuminanceScale * mask, (1.0 / min(BufferSize.x, BufferSize.y)));
    
    // --- Quad Coordinates 0 to 3 ---
    float2 QuadCoords[4] =
    {
        float2(0, 0), float2(1,0), float2(1,1), float2(0,1)
    };
    
    float2 uv = QuadCoords[in.QuadIndex];
    float  AngleOffset = (TilePosition.x / BufferSize.x * 2.0 - 1.0) * 0.523599;
    float AngleBase[3] = {
        AngleOffset + 1.570796,
        AngleOffset + 0.523599,
        AngleOffset + 2.617994
    };
    
    float QuadAngle = AngleBase[in.QuadSet] * GlareScales[in.QuadSet].x;
    
    // --- Compute Rotated Position ---
    float2 position = uv - 0.5;
    float2 rotated  = float2(position.x * cos(QuadAngle) - position.y * sin(QuadAngle),
                             position.x * sin(QuadAngle) + position.y * cos(QuadAngle));
    rotated *= scale * BufferRatio;
    rotated *= (TilePosition - 0.25) / BufferSize * 2.0 - 1.0;
    
    return float4(rotated, 0.0, 1.0);
}

fragment float3 ValhallaGlarePS(float2 uv [[stage_in]],
                                texture2d<float> GlareTexture [[texture(0)]],
                                sampler GlareSampler [[sampler(0)]],
                                float3 InColor)
{
    float3 mask = GlareTexture.sample(GlareSampler, uv).rgb;
    return mask * InColor;
}

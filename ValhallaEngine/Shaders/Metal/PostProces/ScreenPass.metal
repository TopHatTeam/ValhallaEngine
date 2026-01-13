// -------------------------------------------------
//
//  MIT License
//
//  ScreenPass.metal
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/1/26.
//
// -------------------------------------------------

#include <metal_stdlib>
using namespace metal;

struct InVertex
{
    float2 position [[attribute(0)]];
    float2 texcoord [[attribute(1)]];
};

// NOTICE!
// ------------------------------------------------------------------
// Metal does not support 'noperspective' like DirectX does
// So the only work around is to split the baby for 'UVAndScreenPos'
// I'm sure they'll be a happy couple!
struct OutVertex
{
    float4 position             [[position]]; // SV_POSITION Metal equivalent
    float2 uv;
    float2 screenpos;           // very explicit screen space!
};

vertex OutVertex ValhallaScreenPassVS(InVertex in [[stage_in]])
{
    OutVertex out;
    
    // This whole shader is from a Unreal Engine Shader which is designed around DirectX
    // So we don't have 'DrawRectangle'
    // Here's the best alternative
    
    out.position = float4(in.position, 0.0, 1.0);
    out.uv = in.texcoord;
    out.screenpos = in.position * 0.5 + 0.5; // convert the prick to have no perspective
    
    return out;
}

// -------------------------------------------------
//
//  MIT License
//
//  ChromaShift.frag
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/16/26.
//
// -------------------------------------------------

#version 450
#pragma shader_stage( fragment )

layout (binding = 0) uniform sampler2D InputSampler;

layout (location = 0) in vec4 uPosition;
layout (location = 1) in vec2 UV;
layout (location = 2) in float ChromaShift;

layout (location = 0) out vec3 vColor;

void main()
{
    vec2 uv = UV;
    vec2 center = vec2(0.5, 0.5);
    
    // --- Chromatic Offsets ---
    vec2 UVr = (uv - center) * (1.0 + ChromaShift) + center;
    vec2 UVb = (uv - center) * (1.0 - ChromaShift) + center;
    
    float r = texture(InputSampler, UVr).r;
    float g = texture(InputSampler, uv).g;
    float b = texture(InputSampler, UVb).b;
    
    vColor = vec3(r, g, b);
}

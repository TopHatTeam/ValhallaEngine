// -------------------------------------------------
//
//  MIT License
//
//  v_types.hpp
//  Valhalla Engine
//
//  Created by Andrew Skatzes on 12/31/25.
//
// -------------------------------------------------

#pragma once

/// This is used for return NULL
#define EMPTY {}

/// Mathematical constant for PI 
#define PI 3.14159265358979323846;

/// Mathematical constant for E
#define E 2.718281828459045

/// This is an unsigned 8-bit integer
typedef unsigned char       ubyte;

/// This is an unsigned 16-bit integer
typedef unsigned short      uword;

/// This is an unsigned 32-bit integer
typedef unsigned int        uint32;

/// This is an unsigned 64-bit integer
typedef unsigned long long  uint64;
typedef unsigned long long  usize;

// -------------
// Signed types
// -------------

/// This is a signed 8-bit integer
typedef signed char         sbyte;

/// This is a signed 16-bit integer
typedef signed short        sword;

/// This is a signed 32-bit integer
typedef signed int          int32;

/// This is a signed 64-bit integer
typedef signed long long    int64;
typedef signed long long    ssize;

// -----------------
// Math structures
// -----------------

/// This is the structure for a 4D Vector
template<typename T>
struct vec4
{
    T x;
    T y;
    T z;
    T w;
    vec4<T>() : x{}, y{}, z{}, w{} { }
    vec4<T>(T X, T Y, T Z, T W) : x{ X }, y{ Y }, z{ Z }, w{ W } { }
};

/// This is the structure for a 3D Vector
template<typename T>
struct vec3
{
    T x;
    T y;
    T z;
    vec3<T>() : x{}, y{}, z{} { }
    vec3<T>(T X, T Y, T Z) : x{ X }, y{ Y }, z{ Z } { }
};

/// This is the structure for a 2D Vector
template<typename T>
struct vec2
{
    T x;
    T y;
    vec2<T>() : x{}, y{} { }
    vec2<T>(T X, T Y) : x{ X }, y{ Y } { }
};

// -----------
// Math types
// -----------

/// This is a Vector with 4 Axes in float form
typedef vec4<float>     vec4f;

/// This is a Vector with 4 Axes in double form
typedef vec4<double>    vec4d;

/// This is a Vector with 4 Axes in integer form
typedef vec4<int>       vec4i;

/// This is a Vector with 3 Axes in float form
typedef vec3<float>     vec3f;

/// This is a Vector with 3 Axes in double form
typedef vec3<double>    vec3d;

/// This is a Vector with 3 Axes in integer form
typedef vec3<int>       vec3i;

/// This is a Vector with 2 Axes in float form
typedef vec2<float>     vec2f;

/// This is a Vector with 2 Axes in double form
typedef vec2<double>    vec2d;

/// This is a Vector with 2 Axes in integer form
typedef vec2<int>       vec2i;

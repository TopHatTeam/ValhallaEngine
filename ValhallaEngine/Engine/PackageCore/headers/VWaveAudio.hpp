// -------------------------------------------------
//
//  MIT License
//
//  VWaveAudio.hpp
//  Valhalla Engine
//
//  Created by Andrew Skatzes on 10/17/25.
//
// -------------------------------------------------

#pragma once

#include "../../Core/headers/VTypes.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

// ok we're going try to detect big-endian and little-endian 
// I know most modern programmers probably don't give a fuck about it
// I do because wav parsing is something we HAVE TO GIVE A FUCK
// so here's my black magic spell

#define LITTLE_ENDIAN_ORDER     0x44434241UL
#define BIG_ENDIAN_ORDER        0x41424344UL

namespace ValhallaEngine::Endian
{
    inline constexpr bool IsHostBigEndian()
    {
        union 
        {
            char bytes[4];
            uint32 EndianID;
        } EndianOrder = { {'A', 'B', 'C', 'D'} };

        return EndianOrder.EndianID == BIG_ENDIAN_ORDER;
    }

    inline constexpr bool IsHostLittleEndian()
    {
        union 
        {
            char bytes[4];
            uint32 EndianID;
        } EndianOrder = { {'A', 'B', 'C', 'D'} };

        return EndianOrder.EndianID == LITTLE_ENDIAN_ORDER;
    }
}

namespace ValhallaEngine::Audio
{
    inline constexpr uint32 VBigEndianWavRIFFChunkID    = ((uint32)'FFIR');
    inline constexpr uint32 VBigEndianWavFormatChunkID  = ((uint32)' tmf');
    inline constexpr uint32 VBigEndianWavFactChunkID    = ((uint32)'tcaf');
    inline constexpr uint32 VBigEndianWavDataChunkID    = ((uint32)'atad');
    inline constexpr uint32 VBigEndianWavID             = ((uint32)'EVAW');

    inline constexpr uint32 VLittleEndianWavRIFFChunkID     = ((uint32)'RIFF');
    inline constexpr uint32 VLittleEndianWavFormatChunkID   = ((uint32)'fmt ');
    inline constexpr uint32 VLittleEndianWavFactChunkID     = ((uint32)'fact');
    inline constexpr uint32 VLittleEndianWavDataChunkID     = ((uint32)'data');
    inline constexpr uint32 VLittleEndianWavID              = ((uint32)'WAVE');

    #pragma pack(push, 1)
    struct VWavChunkHeader
    {
        uint32 id;
        uint32 size;
    };

    struct VWavFormatChunk
    {
        VWavChunkHeader WavHeader;
        uint64 offset;

        struct VWavBody
        {
            uword FormatTag;
            uword NumberOfChannels;
            uint32 SampleRate;
            uint32 AverageBytesPerSec;
            uword BlockAlignment;
            uword BitsPerSample;
            uword ExtensionSize;
            uword ValidBitsPerSample;
            uint32 ChannelMask;
            ubyte SubFormat[16];
        };

        VWavBody WavBody;
    };

    struct VWavFactChunk 
    {
        VWavChunkHeader WavHeader;
        uint64 offset;

        struct VWavBody 
        {
            uint32 SampleLength;
        };

        VWavBody WavBody;
    };

    struct VWavDataChunk
    {
        VWavChunkHeader WavHeader;
        uint64 offset;
    };

    struct VWavMasterChunk
    {
        uint32 id;
        uint32 size;
        uint32 WavID;
        uint64 offset;
    };

    struct VWavFile 
    {
        FILE* WavFile;
        char* Filename;
        bool bIsNewFile;

        VWavMasterChunk RiffChunk;
        VWavFormatChunk FormatChunk;
        VWavFactChunk FactChunk;
        VWavDataChunk DataChunk;
    };

    #pragma pack(pop)

    constexpr ubyte VWavDefaultSubFormat[16] = 
    {
        0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x10, 0x00,
        0x80, 0x00, 0x00, 0xAA,
        0x00, 0x38, 0x9B, 0x71
    };

    bool ParseWavHeader(VWavFile* file);
}
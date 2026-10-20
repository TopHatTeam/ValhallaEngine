// -------------------------------------------------
//
//  MIT License
//
//  VWaveAudio.cpp
//  Valhalla Engine
//
//  Created by Andrew Skatzes on 10/17/25.
//
// -------------------------------------------------

#include "headers/VWaveAudio.hpp"
#include <chrono>

using namespace ValhallaEngine;
using namespace ValhallaEngine::Endian;

using std::min;

bool Audio::ParseWavHeader(VWavFile* file)
{
    usize ReadCount;
    const char* FileType = "WAVE";

    ReadCount = fread(&file->RiffChunk, sizeof(VWavChunkHeader), 1, file->WavFile);
    CHECK_EOF(ReadCount, FileType);

    if (file->RiffChunk.id != VBigEndianWavRIFFChunkID && file->RiffChunk.id != VLittleEndianWavRIFFChunkID)
    {
        fmt::println("IO Error: Formatting error, expected file is not a RIFF file");
        return false;
    }

    ReadCount = fread(&file->RiffChunk.WavID, 4, 1, file->WavFile);
    CHECK_EOF(ReadCount, FileType);

    if (file->RiffChunk.WavID != VBigEndianWavID && file->RiffChunk.WavID != VLittleEndianWavID)
    {
        fmt::println("IO Error: Formatting error, expected file is not a WAVE file");
        return false;
    }

    file->RiffChunk.offset = (uint64)ftell(file->WavFile);

    while (file->DataChunk.WavHeader.id != VBigEndianWavDataChunkID && file->DataChunk.WavHeader.id != VLittleEndianWavDataChunkID)
    {
        VWavChunkHeader Header; 

        ReadCount = fread(&Header, sizeof(VWavChunkHeader), 1, file->WavFile);
        CHECK_EOF(ReadCount, FileType);

        if (IsHostBigEndian() == true)
        {
            switch (Header.id)
            {
            case VBigEndianWavFormatChunkID: 
                file->FormatChunk.WavHeader = Header;
                file->FormatChunk.offset = (uint64)ftell(file->WavFile);

                ReadCount = fread(&file->FormatChunk.WavBody, min(Header.size, static_cast<uint32>(sizeof(file->FormatChunk.WavBody))), 1, file->WavFile);
                CHECK_EOF(ReadCount, FileType);
                
                
            }
        }
        else if (IsHostLittleEndian() == true)
        {
            switch (Header.id)
            {
            case VLittleEndianWavFormatChunkID:
                file->FormatChunk.WavHeader = Header;
                file->FormatChunk.offset = (uint64)ftell(file->WavFile);

                ReadCount = fread(&file->FormatChunk.WavBody, min(Header.size, static_cast<uint32>(sizeof(file->FormatChunk.WavBody))), 1, file->WavFile);
                CHECK_EOF(ReadCount, FileType);

                
            }
        }
    }

}
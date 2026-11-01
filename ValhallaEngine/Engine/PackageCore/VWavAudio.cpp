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

// ----------------------------------------------------------
// Credit to brglng and his libwave github for this section:
// https://github.com/brglng/libwave/tree/master
// ----------------------------------------------------------

#include "headers/VWaveAudio.hpp"
#include <chrono>

using namespace ValhallaEngine;
using namespace ValhallaEngine::Endian;

using std::min;

WAVTHREAD Audio::VWavError WError = {Audio::VWavErrorCode::VWEC_OK, 1};

constexpr void VWavErrorClear(void)
{
    if (WError.ErrorCode != Audio::VWavErrorCode::VWEC_OK)
    {   
        WError.ErrorCode    = Audio::VWavErrorCode::VWEC_OK;
        WError.IsLiteral    = 1;
    }
}

constexpr void VWavErrorSet(Audio::VWavErrorCode code, int literal)
{
    assert(WError.ErrorCode == Audio::VWavErrorCode::VWEC_OK);
    WError.ErrorCode = code;
    WError.IsLiteral = literal;
}

bool Audio::ParseWavHeader(VWavFile* file)
{
    usize ReadCount;

    ReadCount = fread(&file->RiffChunk, sizeof(VWavChunkHeader), 1, file->WavFile);
    if (ReadCount != 1)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 1);
        fmt::println("IO ERROR: Unexpected End of File for Wav file format");
        return false;
    }

    if (file->RiffChunk.id != VBigEndianWavRIFFChunkID && file->RiffChunk.id != VLittleEndianWavRIFFChunkID)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 1);
        fmt::println("IO Error: Formatting error, expected file is not a RIFF file");
        return false;
    }

    ReadCount = fread(&file->RiffChunk.WavID, 4, 1, file->WavFile);
    if (ReadCount != 1)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 1);
        fmt::println("IO ERROR: Unexpected End of File for Wav file format");
        return false;
    }

    if (file->RiffChunk.WavID != VBigEndianWavID && file->RiffChunk.WavID != VLittleEndianWavID)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 1);
        fmt::println("IO Error: Formatting error, expected file is not a WAVE file");
        return false;
    }

    file->RiffChunk.offset = (uint64)ftell(file->WavFile);

    while (file->DataChunk.WavHeader.id != VBigEndianWavDataChunkID && file->DataChunk.WavHeader.id != VLittleEndianWavDataChunkID)
    {
        VWavChunkHeader Header; 

        ReadCount = fread(&Header, sizeof(VWavChunkHeader), 1, file->WavFile);
        if (ReadCount != 1)
        {
            VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 1);
            fmt::println("IO ERROR: Unexpected End of File for Wav file format");
            return false;
        }

        if (IsHostBigEndian() == true)
        {
            switch (Header.id)
            {
            case VBigEndianWavFormatChunkID: 
                file->FormatChunk.WavHeader = Header;
                file->FormatChunk.offset = (uint64)ftell(file->WavFile);

                ReadCount = fread(&file->FormatChunk.WavBody, min(Header.size, static_cast<uint32>(sizeof(file->FormatChunk.WavBody))), 1, file->WavFile);
                if (ReadCount != 1)
                {
                    VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 1);
                    fmt::println("IO ERROR: Unexpected End of File for Wav file format");
                    return false;
                }
            
                if (file->FormatChunk.WavBody.FormatTag != VWavFormatPCM &&
                    file->FormatChunk.WavBody.FormatTag != VWavFormatIEEEFloat &&
                    file->FormatChunk.WavBody.FormatTag != VWavFormatALAW &&
                    file->FormatChunk.WavBody.FormatTag != VWavFormatMULAW
                )
                {
                    VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 0);
                    fmt::println("IO ERROR: Unsupported Format Tag: {} in WAV file format", file->FormatChunk.WavBody.FormatTag);
                    return false;
                }
                break;
            
            case VBigEndianWavFactChunkID:
                file->FactChunk.WavHeader = Header;
                file->FactChunk.offset = (uint64)ftell(file->WavFile);

                ReadCount = fread(&file->FactChunk.WavBody, min(Header.size, static_cast<uint32>(sizeof(file->FactChunk.WavBody))), 1, file->WavFile);
                if (ReadCount != 1)
                {
                    VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 0);
                    fmt::println("IO ERROR: Unexpected End of File for Wav file format");
                    return false;
                }
                break;
            
            case VBigEndianWavDataChunkID:
                file->DataChunk.WavHeader = Header;
                file->DataChunk.offset = (uint64)ftell(file->WavFile);
                break;

            default:
                if (fseek(file->WavFile, Header.size, SEEK_CUR) < 0)
                {
                    VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                    fmt::println("IO ERROR: File seeking failed with (errno {}: {})", errno, strerror(errno));
                    return false;
                }
                break;
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
                if (ReadCount != 1)
                {
                    VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 1);
                    fmt::println("IO ERROR: Unexpected End of File for Wav file format");
                    return false;
                }

                if (file->FormatChunk.WavBody.FormatTag != VWavFormatPCM &&
                    file->FormatChunk.WavBody.FormatTag != VWavFormatIEEEFloat &&
                    file->FormatChunk.WavBody.FormatTag != VWavFormatALAW &&
                    file->FormatChunk.WavBody.FormatTag != VWavFormatMULAW
                )
                {
                    VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 0);
                    fmt::println("IO ERROR: Unsupported Format Tag: {} in WAV file format", file->FormatChunk.WavBody.FormatTag);
                    return false;
                }
                break;

            case VLittleEndianWavFactChunkID:
                file->FactChunk.WavHeader = Header;
                file->FactChunk.offset = (uint64)ftell(file->WavFile);

                ReadCount = fread(&file->FactChunk.WavBody, min(Header.size, static_cast<uint32>(sizeof(file->FactChunk.WavBody))), 1, file->WavFile);
                if (ReadCount != 1)
                {
                    VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 0);
                    fmt::println("IO ERROR: Unexpected End of File for Wav file format");
                    return false;
                }
                break;

            case VLittleEndianWavDataChunkID:
                file->DataChunk.WavHeader = Header;
                file->DataChunk.offset = (uint64)ftell(file->WavFile);
                break;

            default:
                if (fseek(file->WavFile, Header.size, SEEK_CUR) < 0)
                {
                    VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                    fmt::println("IO ERROR: File seeking failed with (errno {}: {})", errno, strerror(errno));
                    return false;
                }
                break;

            }
        }
    }

    return true;
}

bool Audio::WriteWavHeader(VWavFile* file)
{
    if (IsHostBigEndian() == true)
    {
              file->RiffChunk.size = sizeof(file->RiffChunk.WavID) + 
        (file->FormatChunk.WavHeader.id == VBigEndianWavFormatChunkID ? (sizeof(VWavChunkHeader) + file->FormatChunk.WavHeader.size) : 0) +
        (file->FactChunk.WavHeader.id == VBigEndianWavFactChunkID ? (sizeof(VWavChunkHeader) + file->FactChunk.WavHeader.size) : 0) +
        (file->DataChunk.WavHeader.id == VBigEndianWavDataChunkID ? (sizeof(VWavChunkHeader) + file->DataChunk.WavHeader.size) : 0);
        
        if (fseek(file->WavFile, 0, SEEK_SET) != 0)
        {
            VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
            fmt::println("IO ERROR: File seeking failed with (errno {}: {})", errno, strerror(errno));
            return false;
        }

        if (fwrite(&file->RiffChunk, sizeof(VWavChunkHeader) + 4, 1, file->WavFile) != 1)
        {
            VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
            fmt::println("IO ERROR: Error while writing to filepath {} with (errno: {}: {})", file->Filename, errno, strerror(errno));
            return false;
        }

        if (file->FormatChunk.WavHeader.id == VBigEndianWavFormatChunkID)
        {
            if (fseek(file->WavFile, (long)(file->FormatChunk.offset - sizeof(VWavChunkHeader)), SEEK_SET) != 0)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: File seeking failed with (errno {}: {})", errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->FormatChunk.WavHeader, sizeof(VWavChunkHeader), 1, file->WavFile) != 1) 
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: An error occured while writing to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->FormatChunk.WavBody, file->FormatChunk.WavHeader.size, 1, file->WavFile) != 1)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: An error occured while writing to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
                return false;
            }
        }

        if (file->FactChunk.WavHeader.id == VBigEndianWavFactChunkID)
        {
            if (fseek(file->WavFile, (long)(file->FactChunk.offset - sizeof(VWavChunkHeader)), SEEK_SET) != 0)
            {
                fmt::println("IO ERROR: File seeking failed with (errno {}: {})", errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->FactChunk.WavHeader, sizeof(VWavChunkHeader), 1, file->WavFile) != 1) 
            {
                fmt::println("IO ERROR: An error occured while writing to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->FactChunk.WavBody, file->FactChunk.WavHeader.size, 1, file->WavFile) != 1)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: An error occured while writing to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
                return false;
            }

        }

        if (file->DataChunk.WavHeader.id == VBigEndianWavDataChunkID)
        {
            if (fseek(file->WavFile, (long)(file->DataChunk.offset - sizeof(VWavChunkHeader)), SEEK_SET) != 0)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: File seeking failed with (errno {}: {})", errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->DataChunk.WavHeader, sizeof(VWavChunkHeader), 1, file->WavFile) != 1)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: An error occured while writing to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
                return false;
            }
        }

        return true;
    }
    else if (IsHostLittleEndian() == true)
    {
        file->RiffChunk.size = sizeof(file->RiffChunk.WavID) + 
        (file->FormatChunk.WavHeader.id == VLittleEndianWavFormatChunkID ? (sizeof(VWavChunkHeader) + file->FormatChunk.WavHeader.size) : 0) +
        (file->FactChunk.WavHeader.id == VLittleEndianWavFactChunkID ? (sizeof(VWavChunkHeader) + file->FactChunk.WavHeader.size) : 0) +
        (file->DataChunk.WavHeader.id == VLittleEndianWavDataChunkID ? (sizeof(VWavChunkHeader) + file->DataChunk.WavHeader.size) : 0);
        
        if (fseek(file->WavFile, 0, SEEK_SET) != 0)
        {
            VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
            fmt::println("IO ERROR: File seeking failed with (errno {}: {})", errno, strerror(errno));
            return false;
        }

        if (fwrite(&file->RiffChunk, sizeof(VWavChunkHeader) + 4, 1, file->WavFile) != 1)
        {
            VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
            fmt::println("IO ERROR: Error while writing to filepath {} with (errno: {}: {})", file->Filename, errno, strerror(errno));
            return false;
        }

        if (file->FormatChunk.WavHeader.id == VLittleEndianWavFormatChunkID)
        {
            if (fseek(file->WavFile, (long)(file->FormatChunk.offset - sizeof(VWavChunkHeader)), SEEK_SET) != 0)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: File seeking failed with (errno {}: {})", errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->FormatChunk.WavHeader, sizeof(VWavChunkHeader), 1, file->WavFile) != 1) 
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: An error occured while writing to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->FormatChunk.WavBody, file->FormatChunk.WavHeader.size, 1, file->WavFile) != 1)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: An error occured while writing to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
                return false;
            }
        }

        if (file->FactChunk.WavHeader.id == VLittleEndianWavFactChunkID)
        {
            if (fseek(file->WavFile, (long)(file->FactChunk.offset - sizeof(VWavChunkHeader)), SEEK_SET) != 0)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: File seeking failed with (errno {}: {})", errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->FactChunk.WavHeader, sizeof(VWavChunkHeader), 1, file->WavFile) != 1) 
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: An error occured while writing to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->FactChunk.WavBody, file->FactChunk.WavHeader.size, 1, file->WavFile) != 1)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
                fmt::println("IO ERROR: An error occured while writing to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
                return false;
            }

        }

        if (file->DataChunk.WavHeader.id == VLittleEndianWavDataChunkID)
        {
            if (fseek(file->WavFile, (long)(file->DataChunk.offset - sizeof(VWavChunkHeader)), SEEK_SET) != 0)
            {
                fmt::println("IO ERROR: File seeking failed with (errno {}: {})", errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->DataChunk.WavHeader, sizeof(VWavChunkHeader), 1, file->WavFile) != 1)
            {
                fmt::println("IO ERROR: An error occured while writing to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
                return false;
            }
        }

        return true;
    }   
    else 
    {
        fmt::println("IO ERROR: CPU doesn't support Big-Endian or Little-Endian. Unknown Endian-ness.");
        return false;
    }
}

bool Audio::InitializeWav(VWavFile* file, const char* filename, uint32 WavMode)
{
    memset(file, 0, sizeof(VWavFile));

    if (WavMode & VWavOpenRead)
    {
        if ((WavMode & VWavOpenWrite) || (WavMode & VWavOpenAppend))
        {
            file->WavFile = fopen(filename, "wb+");
        }
        else 
        {
            file->WavFile = fopen(filename, "rb");
        }
    }
    else 
    {
        if ((WavMode & VWavOpenWrite) || (WavMode & VWavOpenAppend))
        {
            file->WavFile = fopen(filename, "wb+");
        }
        else 
        {
            VWavErrorSet(VWavErrorCode::VWEC_ERROR_PARAMETER, 1);
            fmt::println("IO ERROR: Invalid mode for Wave file");
            return false;
        }
    }

    if (file->WavFile == nullptr)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 0);
        fmt::println("IO ERROR: An error occured while opening to {} with (errno {}: {})", file->Filename, errno, strerror(errno));
        return false;
    }

    file->Filename  = strdup(filename);
    file->WavMode   = WavMode;

    if (!(file->WavMode & VWavOpenWrite) && !(file->WavMode & VWavOpenAppend))
    {
        ParseWavHeader(file);
        return true;
    }

    if (file->WavMode & VWavOpenAppend)
    {
        ParseWavHeader(file);
        if (WError.ErrorCode == VWavErrorCode::VWEC_OK)
        {
            return true; 
        }
        else 
        {
            VWavErrorClear();
            rewind(file->WavFile);
            file->bIsNewFile = true;
        }
    }

    /// We reach here only for creating a new file

    if (IsHostBigEndian())
    {
        file->RiffChunk.id      = VBigEndianWavRIFFChunkID;
        file->RiffChunk.WavID   = VBigEndianWavID;
        file->RiffChunk.offset  = sizeof(VWavChunkHeader) + 4;

        file->FormatChunk.WavHeader.id                  = VBigEndianWavFormatChunkID;
        file->FormatChunk.WavHeader.size                = (uint32)(usize)&file->FormatChunk.WavBody.ExtensionSize - (usize)&file->FormatChunk.WavBody;
        file->FormatChunk.offset                        = file->RiffChunk.offset + sizeof(VWavChunkHeader);
        file->FormatChunk.WavBody.FormatTag             = VWavFormatPCM;
        file->FormatChunk.WavBody.NumberOfChannels      = 2;
        file->FormatChunk.WavBody.SampleRate            = 44100;
        file->FormatChunk.WavBody.AverageBytesPerSec    = 44100 * 2 * 2;
        file->FormatChunk.WavBody.BlockAlignment        = 4;
        file->FormatChunk.WavBody.BitsPerSample         = 16;

        memcpy(file->FormatChunk.WavBody.SubFormat, VWavDefaultSubFormat, 16);

        file->DataChunk.WavHeader.id    = VBigEndianWavDataChunkID;
        file->DataChunk.offset          = file->FormatChunk.offset + file->FormatChunk.WavHeader.size + sizeof(VWavChunkHeader);

        WriteWavHeader(file);
    }
    else if (IsHostLittleEndian())
    {
        file->RiffChunk.id      = VLittleEndianWavRIFFChunkID;
        file->RiffChunk.WavID   = VLittleEndianWavID;
        file->RiffChunk.offset  = sizeof(VWavChunkHeader) + 4;

        file->FormatChunk.WavHeader.id                  = VLittleEndianWavFormatChunkID;
        file->FormatChunk.WavHeader.size                = (uint32)(usize)&file->FormatChunk.WavBody.ExtensionSize - (usize)&file->FormatChunk.WavBody;
        file->FormatChunk.offset                        = file->RiffChunk.offset + sizeof(VWavChunkHeader);
        file->FormatChunk.WavBody.FormatTag             = VWavFormatPCM;
        file->FormatChunk.WavBody.NumberOfChannels      = 2;
        file->FormatChunk.WavBody.SampleRate            = 44100;
        file->FormatChunk.WavBody.AverageBytesPerSec    = 44100 * 2 * 2;
        file->FormatChunk.WavBody.BlockAlignment        = 4;
        file->FormatChunk.WavBody.BitsPerSample         = 16;

        memcpy(file->FormatChunk.WavBody.SubFormat, VWavDefaultSubFormat, 16);

        file->DataChunk.WavHeader.id    = VLittleEndianWavDataChunkID;
        file->DataChunk.offset          = file->FormatChunk.offset + file->FormatChunk.WavHeader.size + sizeof(VWavChunkHeader);

        WriteWavHeader(file);
    }

}
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
#include <cerrno>

using namespace ValhallaEngine;
using namespace ValhallaEngine::Endian;

using std::min;

WAVTHREAD Audio::VWavError WError = {Audio::VWavErrorCode::VWEC_OK, 1};

inline void VWavErrorClear(void)
{
    if (WError.ErrorCode != Audio::VWavErrorCode::VWEC_OK)
    {   
        WError.ErrorCode    = Audio::VWavErrorCode::VWEC_OK;
        WError.IsLiteral    = 1;
    }
}

inline void VWavErrorSet(Audio::VWavErrorCode code, int literal)
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

bool Audio::FinializeWav(VWavFile* file)
{
    int ret;

    free(file->Filename);

    if (file->WavFile == nullptr)
    {
        fmt::println("IO ERROR: Wav file return a NULL value.");
        return false;
    }

    ret = fclose(file->WavFile);
    if (ret != 0)
    {
        fmt::println("IO ERROR: Failed to close Wav file with error code {}: errno {}: {}", ret, errno, strerror(errno));
        return false;
       }
        
    return true;
}

Audio::VWavFile* Audio::OpenWav(const char* filename, uint32 WavMode)
{
    VWavFile* file = reinterpret_cast<VWavFile*>(malloc(sizeof(VWavFile)));
    if (file == nullptr)
    {
        return nullptr;
    }

    InitializeWav(file, filename, WavMode);
    return file;
}

Audio::VWavFile* Audio::ReopenWav(VWavFile* file, const char* filename, uint32 WavMode)
{
    FinializeWav(file);
    InitializeWav(file, filename, WavMode);
    return file;
}

bool Audio::CloseWav(VWavFile* file)
{
    FinializeWav(file);
    free(file);

    return true;
}

int Audio::SeekWav(VWavFile* file, long int offset, int origin)
{
    usize length = GetLengthWav(file);
    int ret;

    if (origin == SEEK_CUR)
    {
        offset += reinterpret_cast<long>(ftell(file->WavFile));
    }
    else if (origin == SEEK_END)
    {
        offset += static_cast<long>(length);
    }

        // POSIX allows for seeking beyond the end of file... I don't make the rules
    if (offset >= 0)
    {
        offset *= file->FormatChunk.WavBody.BlockAlignment;
    }
    else 
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_PARAMETER, 0); // literal
        fmt::println("IO ERROR: Invalid seek offset {}", offset);
        return int(WError.ErrorCode); // reinterpret_cast doesn't work here so we have to use the classic C-way of casting.
    }

    ret = fseek(file->WavFile, (long)file->DataChunk.offset + offset, SEEK_SET);
    if (ret != 0)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
        fmt::println("IO ERROR: fseek() failed with errno {}: {}", errno, strerror(errno));
        return (int)ret;
    }

    return 0;
}

void Audio::RewindWav(VWavFile* file)
{
    SeekWav(file, 0, SEEK_SET);
}

int Audio::EOFWav(const VWavFile* file)
{
    return feof(file->WavFile) || ftell(file->WavFile) == static_cast<long>(file->DataChunk.offset + file->DataChunk.WavHeader.size);
}

int Audio::FlushWav(VWavFile* file)
{
    int ret = fflush(file->WavFile);

    if (ret != 0)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
        fmt::println("IO ERROR: fflush() failed with errno {}: {}", errno, strerror(errno));
    }

    return ret;
}

usize Audio::ReadWav(VWavFile* file, void* buffer, usize count)
{
    usize ReadCount;
    uword NumberOfChannels = GetNumberOfChannelsWav(file);
    usize SampleSize       = GetSampleSizeWav(file);
    usize LengthRemaining;

    if (!(file->WavMode & VWavOpenRead))
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_MODE, 0); // literal
        fmt::println("IO ERROR: Wav file is not readable");
        return 0;
    }

    if (file->FormatChunk.WavBody.FormatTag == VWavFormatExtensible)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 0); // literal
        fmt::println("IO ERROR: Wav file is in extensible format, it is not supported");
        return 0;
    }
}

constexpr bool Audio::UpdateSizesWav(VWavFile* file)
{
    long int SavePosition = ftell(file->WavFile);

    if (fseek(file->WavFile, static_cast<long>(sizeof(VWavChunkHeader) - 4), SEEK_SET) != 0)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
        fmt::println("IO ERROR: fseek() failed with errno {}: {}", errno, strerror(errno));
        return false;
    }

    if (fwrite(&file->RiffChunk.size, 4, 1, file->WavFile) != 1)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
        fmt::println("IO ERROR: fwrite() failed with errno {}: {}", errno, strerror(errno));
        return false;
    }

    if (IsHostLittleEndian() == true)
    {
        if (file->FactChunk.WavHeader.id == VLittleEndianWavFactChunkID)
        {
            if (fseek(file->WavFile, static_cast<long>(file->FactChunk.offset), SEEK_SET) != 0)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
                fmt::println("IO ERROR: fseek() failed with errno {}: {}", errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->FactChunk.WavBody.SampleLength, 4, 1, file->WavFile) != 1)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
                fmt::println("IO ERROR: fwrite() failed with errno {}: {}", errno, strerror(errno));
                return false;
            }
        }
    }
    else if (IsHostBigEndian() == true)
    {
        if (file->FactChunk.WavHeader.id == VBigEndianWavFactChunkID)
        {
            if (fseek(file->WavFile, static_cast<long>(file->FactChunk.offset), SEEK_SET) != 0)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
                fmt::println("IO ERROR: fseek() failed with errno {}: {}", errno, strerror(errno));
                return false;
            }

            if (fwrite(&file->FactChunk.WavBody.SampleLength, 4, 1, file->WavFile) != 1)
            {
                VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
                fmt::println("IO ERROR: fwrite() failed with errno {}: {}", errno, strerror(errno));
                return false;
            }
        }
    }

    if (fseek(file->WavFile, static_cast<long>(file->DataChunk.offset - 4), SEEK_SET) != 0)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
        fmt::println("IO ERROR: fseek() failed with errno {}: {}", errno, strerror(errno));
        return false;
    }

    if (fwrite(&file->DataChunk.WavHeader.size, 4, 1, file->WavFile) != 1)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
        fmt::println("IO ERROR: fwrite() failed with errno {}: {}", errno, strerror(errno));
        return false;
    }

    if (fseek(file->WavFile, SavePosition, SEEK_SET) != 0)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
        fmt::println("IO ERROR: fseek() failed with errno {}: {}", errno, strerror(errno));
       return false;
    }

    return true;
}

bool Audio::SetFormatWav(VWavFile* file, uword format)
{
    if (!(file->WavMode & VWavOpenWrite) && !((file->WavMode & VWavOpenAppend) && file->bIsNewFile && file->DataChunk.WavHeader.size == 0)) 
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_MODE, 0); // literal
        return false;
    }

    if (format == file->FormatChunk.WavBody.FormatTag)
    {
        return true;
    }

    file->FormatChunk.WavBody.FormatTag = format;
    if (format != VWavFormatPCM && format != VWavFormatIEEEFloat)
    {
        file->FormatChunk.WavBody.ExtensionSize = 0;
        file->FormatChunk.WavHeader.size = (uint32)((usize)&file->FormatChunk.WavBody.ExtensionSize - (usize)&file->FormatChunk.WavBody);
    }
    else if (format == VWavFormatExtensible)
    {
        file->FormatChunk.WavBody.ExtensionSize = 22;
        file->FormatChunk.WavHeader.size = sizeof(VWavFormatChunk) - sizeof(VWavChunkHeader);
    }

    if (format == VWavFormatALAW || format == VWavFormatMULAW)
    {
        uword SampleSize = GetSampleSizeWav(file);
        if (SampleSize != 1)
        {
            SetSampleSizeWav(file, 1);
        }
    }
    else if (format == VWavFormatIEEEFloat)
    {
        uword SampleSize = GetSampleSizeWav(file);
        if (SampleSize != 4 && SampleSize != 8)
        {
            SetSampleSizeWav(file, 4);
        }
    }

    WriteWavHeader(file);

    return true;
}

bool Audio::SetNumberOfChannelsWav(VWavFile* file, uword NumberOfChannels)
{
    if (!(file->WavMode & VWavOpenWrite) && !((file->WavMode & VWavOpenAppend) && file->bIsNewFile && file->DataChunk.WavHeader.size == 0))
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_MODE, 0); // literal
        fmt::println("IO ERROR: This WAVE file is not writable");
        return false;
    }

    if (NumberOfChannels < 1)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_PARAMETER, 1); // not literal
        fmt::println("IO ERROR: {} number of channels are invalid in a WAVE file", NumberOfChannels);
        return false;
    }

    uword OldNumberOfChannels = file->FormatChunk.WavBody.NumberOfChannels;
    if (NumberOfChannels == OldNumberOfChannels)
    {
        return true;
    }

    file->FormatChunk.WavBody.NumberOfChannels      = NumberOfChannels;
    file->FormatChunk.WavBody.BlockAlignment        = file->FormatChunk.WavBody.BlockAlignment / OldNumberOfChannels * NumberOfChannels;
    file->FormatChunk.WavBody.AverageBytesPerSec    = file->FormatChunk.WavBody.BlockAlignment * file->FormatChunk.WavBody.SampleRate;

    WriteWavHeader(file);

    return true;
}

bool Audio::SetSampleRateWav(VWavFile* file, uint32 SampleRate)
{
    if (!(file->WavMode & VWavOpenWrite) && !((file->WavMode & VWavOpenAppend) && file->bIsNewFile && file->DataChunk.WavHeader.size == 0))
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_MODE, 0); // literal
        fmt::println("IO ERROR: This WAVE file is not writable");
        return false;
    }

    if (SampleRate == file->FormatChunk.WavBody.SampleRate)
    {
        return true;
    }

    file->FormatChunk.WavBody.SampleRate = SampleRate;
    file->FormatChunk.WavBody.AverageBytesPerSec = file->FormatChunk.WavBody.BlockAlignment * file->FormatChunk.WavBody.SampleRate;

    WriteWavHeader(file);
    return true;
}

bool Audio::SetValidBitsPerSampleWav(VWavFile* file, uword bits)
{
    if (!(file->WavMode & VWavOpenWrite) && !((file->WavMode & VWavOpenAppend) && file->bIsNewFile && file->DataChunk.WavHeader.size == 0))
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_MODE, 0); // literal
        fmt::println("IO ERROR: This WAVE file is not writable");
        return false;
    }

    if (bits < 1 || bits > 8 * file->FormatChunk.WavBody.BlockAlignment / file->FormatChunk.WavBody.NumberOfChannels)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_PARAMETER, 1); // not literal
        fmt::println("IO ERROR: {} bits per sample is invalid in a WAVE file", bits);
        return false;
    }

    if ((file->FormatChunk.WavBody.FormatTag == VWavFormatALAW || file->FormatChunk.WavBody.FormatTag == VWavFormatMULAW) && bits != 8)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_PARAMETER, 1); // not literal
        fmt::println("IO ERROR: {} bits per sample is invalid in a WAVE file", bits);
        return false;
    }

    if (file->FormatChunk.WavBody.FormatTag != VWavFormatExtensible)
    {
        file->FormatChunk.WavBody.BitsPerSample = bits;
    }
    else 
    {
        file->FormatChunk.WavBody.BitsPerSample = 8 * file->FormatChunk.WavBody.BlockAlignment / file->FormatChunk.WavBody.NumberOfChannels;
        file->FormatChunk.WavBody.ValidBitsPerSample = bits;
    }

    WriteWavHeader(file);
    
    return true;
}

bool Audio::SetSampleSizeWav(VWavFile* file, usize SampleSize)
{
    if (!(file->WavMode & VWavOpenWrite) && !((file->WavMode & VWavOpenAppend) && file->bIsNewFile && file->DataChunk.WavHeader.size == 0))
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_MODE, 0); // literal
        fmt::println("IO ERROR: This WAVE file is not writable");
        return false;
    }

    if (SampleSize < 1)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_PARAMETER, 1); // not literal
        fmt::println("IO ERROR: Sample size {} is invalid", SampleSize);
        return false;
    }

    file->FormatChunk.WavBody.BlockAlignment = (uword)(SampleSize * file->FormatChunk.WavBody.NumberOfChannels);
    file->FormatChunk.WavBody.AverageBytesPerSec = file->FormatChunk.WavBody.BlockAlignment * file->FormatChunk.WavBody.SampleRate;
    file->FormatChunk.WavBody.BitsPerSample = (uword)(SampleSize * 8);

    if (file->FormatChunk.WavBody.FormatTag == VWavFormatExtensible)
    {
        file->FormatChunk.WavBody.ValidBitsPerSample = (uword)(SampleSize * 8);
    }

    WriteWavHeader(file);

    return true;
}

bool Audio::SetChannelMaskWav(VWavFile* file, uint32 ChannelMask)
{
    if (!(file->WavMode & VWavOpenWrite) && !((file->WavMode & VWavOpenAppend) && file->bIsNewFile && file->DataChunk.WavHeader.size == 0))
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_MODE, 0); // literal
        fmt::println("IO ERROR: This WAVE file is not writable");
        return false;
    }
    
    if (file->FormatChunk.WavBody.FormatTag != VWavFormatExtensible)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 0); // literal
        fmt::println("IO ERROR: Channel mask is only valid for extensible WAVE files");
        return false;
    }

    file->FormatChunk.WavBody.ChannelMask = ChannelMask;

    WriteWavHeader(file);

    return true;
}

bool Audio::SetSubFormatWav(VWavFile* file, uword SubFormat)
{
    if (!(file->WavMode & VWavOpenWrite) && !((file->WavMode & VWavOpenAppend) && file->bIsNewFile && file->DataChunk.WavHeader.size == 0))
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_MODE, 0); // literal
        fmt::println("IO ERROR: This WAVE file is not writable");
        return false;
    }
    
    if (file->FormatChunk.WavBody.FormatTag != VWavFormatExtensible)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_FORMAT, 0); // literal
        fmt::println("IO ERROR: Subformat is only valid for extensible WAVE files");
        return false;
    }

    file->FormatChunk.WavBody.SubFormat[0] = (ubyte)(SubFormat & 0xFF);
    file->FormatChunk.WavBody.SubFormat[1] = (ubyte)(SubFormat >> 8);

    WriteWavHeader(file);

    return true;
}

uword Audio::GetFormatWav(const VWavFile* file)
{
    return file->FormatChunk.WavBody.FormatTag;
}

uword Audio::GetNumberOfChannelsWav(const VWavFile* file)
{
    return file->FormatChunk.WavBody.NumberOfChannels;
}

uint32 Audio::GetSampleRateWav(const VWavFile* file)
{
    return file->FormatChunk.WavBody.SampleRate;
}

uword Audio::GetValidBitsPerSampleWav(const VWavFile* file)
{
    if (file->FormatChunk.WavBody.FormatTag != VWavFormatExtensible)
    {
        return file->FormatChunk.WavBody.BitsPerSample;
    }
    else 
    {
        return file->FormatChunk.WavBody.ValidBitsPerSample;
    }
}

usize Audio::GetSampleSizeWav(const VWavFile* file)
{
    return file->FormatChunk.WavBody.BlockAlignment / file->FormatChunk.WavBody.NumberOfChannels;
}

usize Audio::GetLengthWav(const VWavFile* file)
{
    return file->DataChunk.WavHeader.size / (file->FormatChunk.WavBody.BlockAlignment);
}

uint32 Audio::GetChannelMaskWav(const VWavFile* file)
{
    return file->FormatChunk.WavBody.ChannelMask;
}

uword Audio::GetSubFormatWav(const VWavFile* file)
{
    uword SubFormat = file->FormatChunk.WavBody.SubFormat[1];
    SubFormat <<= 8;
    SubFormat |= file->FormatChunk.WavBody.SubFormat[0];
    return SubFormat;
}

long int Audio::TellWav(const VWavFile* file)
{
    long position = ftell(file->WavFile);

    if (position == -1L)
    {
        VWavErrorSet(VWavErrorCode::VWEC_ERROR_OS, 1); // not literal
        fmt::println("IO ERROR: Failed to get current position in Wav file with error code {}: errno {}: {}", position, errno, strerror(errno));
        return -1L;
    }

    assert(position >= static_cast<long>(file->DataChunk.offset));

    return (long)(((uint64)position - file->DataChunk.offset) / (file->FormatChunk.WavBody.BlockAlignment));
}

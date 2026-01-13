// -------------------------------------------------
//
//  MIT License
//
//  v_datapackage.cpp
//  Valhalla Engine
//
//  Created by Andrew Skatzes on 12/26/25.
//
// -------------------------------------------------

#include "v_datapackage.hpp"

using namespace ValhallaEngine::Package;
using namespace ValhallaEngine::Platform;

VAP::VAP()
{
    file = new VAPFile;
}

VAP::~VAP()
{
    delete file;
}

bool VAP::DoesPackageContainFlags(VAPFile* package)
{
    // Simple bitwise operation to confirm whether this package contains encrypted flag
    // This is a simple 8-bit unsigned integer value
    if (package->header.flags & VAP_ENCRYPTED)
    {
        return true;
    }
    
    if (package->header.flags & VAP_COMPRESSED)
    {
        return true;
    }
    
    return false;
}

bool VAP::WritePackage(const string& filename, const vector<VChunks>& data, ubyte flags)
{
    VHeader PakHeader{};
    PakHeader.ChunkCount    = data.size();
    PakHeader.VersionMajor  = 0;
    PakHeader.VersionMinor  = 1;    // This is a developing engine not a complete one so we're close a major release
    PakHeader.VersionPatch  = 0;
    PakHeader.flags         = flags;
    
    string FinalOutputName = filename + ".vap";
    ofstream outFile(FinalOutputName.c_str(), ios::binary | ios::trunc);
    
    if (!outFile)
    {
        fmt::println("Package ERROR: Unable to write VAP Package. Check file permissions and make sure external drives are plugged in");
        ErrorMessageBox("VAP Package WriteError", "Unable to write VAP Package. Check file permissions and make sure external drives are plugged in");
        return false;
    }
    
    outFile.write(reinterpret_cast<const char*>(&PakHeader), sizeof(PakHeader));
    
    vector<VIndexTable> Table;
    Table.reserve(PakHeader.ChunkCount);
    
    // Write offset
    uint64 offset = sizeof(PakHeader) + PakHeader.ChunkCount * sizeof(VIndexTable);
    
    for (const auto& chunk : data)
    {
        VIndexTable Entry;
        Entry.id        = chunk.id;
        Entry.offset    = offset;
        Entry.size      = chunk.data.size();
        
        Table.push_back(Entry);
        
        offset += Entry.size;
    }
    
    // Write IndexTable
    outFile.write(reinterpret_cast<const char*>(Table.data()), Table.size() * sizeof(VIndexTable));
    
    // Write chunk data
    for (const auto& RawData : data)
    {
        outFile.write(reinterpret_cast<const char*>(RawData.data.data()), RawData.data.size());
    }
    
    outFile.close();
    return true;
}

bool VAP::ReadPackage(const string& filepath)
{
    VHeader PakHeader{};
    ifstream inFile(filepath.c_str(), ios::binary | ios::in);
    
    
    if (!inFile)
    {
        fmt::println("Package ERROR: Unable to read VAP Package at: {}, check file permissions and make sure external drives are plugged in", filepath.c_str());
        ErrorMessageBox("VAP Package ReadError", "Unable to read VAP package at: %s, check file permissions and make sure external drives are plugged in", filepath.c_str());
        return false;
    }
    
    if (!inFile.read(reinterpret_cast<char*>(&PakHeader), (sizeof(PakHeader))))
    {
        fmt::println("Package ERROR: Invalid VAP File, unable to read VAP Package header");
        ErrorMessageBox("VAP Package ReadError", "Invalid VAP File, unable to read VAP Package header");
        return false;
    }
    
    vector<VIndexTable> IndexTable(PakHeader.ChunkCount);
    
    const auto TableBytes = static_cast<std::streamsize>(IndexTable.size() * sizeof(VIndexTable));
    if (TableBytes > 0)
    {
        if (!inFile.read(reinterpret_cast<char*>(IndexTable.data()), TableBytes))
        {
            fmt::println("Package ERROR: Unable to read VAP Package's Index Table. Check if there's any corruption or a bad package");
            ErrorMessageBox("VAP Package ReadError", "Unable to read VAP Package's Index Table. Check if there's any corruption or a bad package");
            return false;
        }
    }
    
    
    //inFile.read(reinterpret_cast<char*>(IndexTable.data()), PakHeader.ChunkCount * sizeof(uint64));
    
    inFile.seekg(0, ios::end);
    const uint64 FileSize = static_cast<uint64>(inFile.tellg());
    const uint64 DataStart = sizeof(VHeader) + PakHeader.ChunkCount * sizeof(VIndexTable);
    
    uint64 PreviousOffset = 0;
    for (usize i = 0; i < IndexTable.size(); i++)
    {
        const auto& EASucks = IndexTable[i];
        
        if (EASucks.offset < DataStart)
        {
            fmt::println("Package ERROR: Data Offset is invalid! Index Offset before data region");
            ErrorMessageBox("VAP Package ReadError", "Data Offset is invalid! Index Offset before data region");
            return false;
        }
        
        if (EASucks.size > (std::numeric_limits<uint64>::max() - EASucks.offset))
        {
            fmt::println("Package ERROR: Package overflow + size!");
            ErrorMessageBox("VAP Package ReadError", "VAP Package overflow!");
            return false;
        }
        
        const uint64 EndOfFile = EASucks.offset + EASucks.size;
        if (EndOfFile > FileSize)
        {
            fmt::println("Package ERROR: Package chunk extends beyond the file size!");
            ErrorMessageBox("VAP Package ReadError", "Package chunk extends beyond the file size!");
            return false;
        }
        
        if (i > 0 && EASucks.offset < PreviousOffset)
        {
            fmt::println("Package ERROR: Package Index offsets are not in ascending order");
            ErrorMessageBox("VAP Package ReadError", "Package Index offsets are not in ascending order");
            return false;
        }
        
        PreviousOffset = EASucks.offset;
    }
    
    vector<VChunks> RetrievedChunks;
    RetrievedChunks.reserve(PakHeader.ChunkCount);
    
    // Read chunk data
    for (usize i = 0; i < PakHeader.ChunkCount; i++)
    {

        const auto& ChunkEntry = IndexTable[i];
        
        // Prepare to retrieve a chunk
        VChunks chunky;
        chunky.id = ChunkEntry.id;
        chunky.data.resize(ChunkEntry.size);
        
        // Seek to the correct chunk offset
        inFile.seekg(ChunkEntry.offset, ios::beg);
        if (!inFile.read(reinterpret_cast<char*>(chunky.data.data()), ChunkEntry.size))
        {
            fmt::println("Package ERROR: Failed to read chunk data at index {}", i);
            ErrorMessageBox("Package ERROR", "Failed to read chunk data at index %d", i);
            return false;
        }
        
        RetrievedChunks.push_back(std::move(chunky));
    }
    
    file->header        = PakHeader;
    file->IndexTable    = std::move(IndexTable);
    file->Chunk         = std::move(RetrievedChunks);
     
    return true;
}

bool DoesFileExist(const char* filepath)
{
    return std::filesystem::exists(filepath);
}


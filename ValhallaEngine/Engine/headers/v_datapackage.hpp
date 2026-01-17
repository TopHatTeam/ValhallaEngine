// -------------------------------------------------
//
//  MIT License
//
//  v_datapackage.h
//  Valhalla Engine
//
//  Created by Andrew Skatzes on 12/26/25.
//
// -------------------------------------------------

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstring>
#include <fstream>

#include "v_types.hpp"
#include "v_platform.h"

#include <fmt/core.h>

/// Used to specify that a `VAP` package has no flags
#define VAP_NOFLAGS    0x00000000

/// Used to specify that a `VAP` package is compressed
#define VAP_COMPRESSED 0x00000001

/// Used to specify that a `VAP` package is encrypted
#define VAP_ENCRYPTED  0x00000010

using std::string, std::vector, std::ios, std::ifstream, std::ofstream;

namespace ValhallaEngine::Package
{
    /**
        `Valhalla Archive Package` header info
        - Note: This isn't a completed for final format. This is always bound to experimential features and possible bugs (Whether big or small) also `NEVER` rewrite this in Rust! I will `NOT` maintain Rust code!
        - Parameter flags: Gives us insight if the `VAP` package has any special features
        - Parameter VersionMajor: Engine major release value
        - Parameter VersionMinor: Engine minor release value
        - Parameter VersionPatch: Engine patch release value
        - Parameter ChunkCount: The amount of chunks in the file
     */
#pragma pack(push, 1)
    struct VHeader
    {
        uint64          ChunkCount;
        uword           VersionMajor;   // unsigned 16-bit integer
        uword           VersionMinor;   // unsigned 16-bit integer
        uword           VersionPatch;   // unsigned 16-bit integer
        ubyte           flags;
    };
#pragma pack(pop)
    
    /**
        `Valhalla Archive Package` index table structure
        - Note: This isn't a completed for final format. This is always bound to experimential features and possible bugs (Whether big or small) also `NEVER` rewrite this in Rust! I will `NOT` maintain Rust code!
        - Parameter id: A unique identifier that tells us where to find the correct chunk
        - Parameter offset: The start of a chunk
        - Parameter size: The size of a chunk
     */
#pragma pack(push, 1)
    struct VIndexTable
    {
        uint32 id;
        uint64 offset;
        uint64 size;
    };
#pragma pack(pop)

    /**
        `Valhalla Archive Package` data chunk
        - Note: This isn't a completed for final format. This is always bound to experimential features and possible bugs (Whether big or small) also `NEVER` rewrite this in Rust! I will `NOT` maintain Rust code!
        - Parameters:
            - id: A unique identifier that tells us where to find the correct chunk
            - data: Contains the raw binary in a vector
     */
#pragma pack(push, 1)
    struct VChunks
    {
        uint32 id;
        vector<ubyte> data;
    };
#pragma pack(pop)

    /**
        `Valhalla Archive Package` file structure
        - Note: This isn't a completed for final format. This is always bound to experimential features and possible bugs (Whether big or small) also `NEVER` rewrite this in Rust! I will `NOT` maintain Rust code!
        - Parameter header: The header structure in the file
        - Parameter IndexTable:  Tells us where the chunk is in the file
        - Parameter Chunk: A structure containing data about the structure
     */
#pragma pack(push, 1)
    struct VAPFile
    {
        VHeader                 header;
        vector<VIndexTable>     IndexTable;
        vector<VChunks>         Chunk;
    };
#pragma pack(pop)
    
    /**
     * This class is for the Valhalla Engine's custom data packaging format
     * `Valhalla Archive Package`, or  simple `VAP`
     *  - Note: This isn't a completed for final format. This is always bound to experimential features and possible bugs (Whether big or small) also `NEVER` rewrite this in Rust! I will `NOT` maintain Rust code!
     *
     *  How does the the `VAP` format work?
     *  Here's how it works:
     *
     *  `| VAP Header | Index Table | Chunks (Data)  |`
     *
     *  Next is how `VAP` chunks work
     *  Here's how it works:
     *
     *  `| Index | Offset | Size | Raw Data (File) | Flags |`
     */
    class VAP
    {
    public:
        /**
            Package contruction. Called when creating the `VAP` class
            - Note :
         */
        VAP();
        
        /**
            Package destruction. Called when destroying the `VAP` class
            - Note : It `WILL` free assets from memory. `DO NOT` try to access assets after this is called. It will cause a `use after free` bug!
         */
        ~VAP();
        
        /**
            Get the private variable `file` so it can be publicly accessed
            - Returns file
         */
        VAPFile* GetPackage() const { return file; }
        
        /**
            Tells us if the package contains any flags
            - Parameter package: Pointer to the package used to help us figure if it has flags
            - Returns True if the package contains flags, otherwise false
         */
        bool DoesPackageContainFlags(VAPFile* package);
        
        /**
            Write data into a `VAP` package file
            - Parameter filename: The name of the file ouput
            - Parameter data: The data chunks that will be used to create the final package
            - Parameter flags: Specifies whether to encrypt or compress the final package
            - Returns True if successfully wrote the `VAP` Package, otherwise false
         */
        bool WritePackage(const string& filename, const vector<VChunks>& data, ubyte flags);
        
        /**
            Reads data from a `VAP` package file and stores in the classes built-in `VAPFile` variable
            - Parameter filepath: The path to the file to open
            - Returns: True if successfully read the `VAP` Package, otherwise false
         */
        bool ReadPackage(const string& filepath);
        
    private:
        VAPFile* file;
    };
    
    /**
        Confirms whether a file exist via a given path
        - Parameter filepath: The path to the file on the drive
        - Returns True if file exists, otherwise false
     */
    bool DoesFileExist(const char* filepath);
}



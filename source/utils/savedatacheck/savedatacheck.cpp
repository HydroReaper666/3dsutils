// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <memory>
#include <cstdlib>
#include <cstdio>

#include <3ds.h>

#include "output.h"
#include "utils/shared_font/shared_font.h"

namespace SaveDataCheck {

std::string BuildSharedRomFSFilename(u32* lowpath) {
    char* filename_buffer;
    asprintf(&filename_buffer, "/%08lx%08lx.bin", lowpath[1], lowpath[0]);
    std::string filename(filename_buffer);
    free(filename_buffer);
    return filename;
}

void DumpSharedRomFS(u32* archive_binary_lowpath) {
    std::string output_file = BuildSharedRomFSFilename(archive_binary_lowpath);

    // Read RomFS bin from SaveDataCheck...

    Handle romfs_handle;
    u64    romfs_size        = 0;
    u32    romfs_bytes_read  = 0;

    FS_archive savedatacheck_archive    = { 0x2345678a, { PATH_BINARY, 16, (u8*)archive_binary_lowpath } };
    u8         file_binary_lowpath[20]  = {};
    FS_path    romfs_path               = { PATH_BINARY, 20, file_binary_lowpath };

    print(GFX_TOP, "Dumping SaveDataCheck RomFS (%s)... ", output_file.c_str());

    FSUSER_OpenFileDirectly(NULL, &romfs_handle, savedatacheck_archive, romfs_path, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
    FSFILE_GetSize(romfs_handle, &romfs_size);

    std::unique_ptr<u8> romfs_data_buffer(new u8[romfs_size]);
    FSFILE_Read(romfs_handle, &romfs_bytes_read, 0, romfs_data_buffer.get(), romfs_size);
    FSFILE_Close(romfs_handle);

    // Dump RomFS bin to SDMC...

    FILE* out_file = fopen(output_file.c_str(), "wb");
    size_t bytes_written = fwrite(romfs_data_buffer.get(), 1, romfs_size, out_file);
    fclose(out_file);

    if (bytes_written == romfs_size)
        print(GFX_TOP, "Done!\n");
    else
        print(GFX_TOP, "Failed!\n");
}

void Dump() {
    // savedatacheck/000400db00010302.bin
    u64 binary_lowpath_000400db00010302[] = { 0x000400db00010302, 0x00000001ffffff00 };
    DumpSharedRomFS((u32*)binary_lowpath_000400db00010302);

    // savedatacheck/0004009b00010202.bin
    u64 binary_lowpath_0004009b00010202[] = { 0x0004009b00010202, 0x00000002ffffff00 };
    DumpSharedRomFS((u32*)binary_lowpath_0004009b00010202);

    // savedatacheck/0004009b00010402.bin
    u64 binary_lowpath_0004009b00010402[] = { 0x0004009b00010402, 0x00000001ffffff00 };
    DumpSharedRomFS((u32*)binary_lowpath_0004009b00010402);
}

} // namespace

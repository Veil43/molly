#ifndef MOLLY_HANDLE_H
#define MOLLY_HANDLE_H

#include "types.h"

#define MOLLY_ASSET_TYPE_COUNT          7u

#define MOLLY_ASSET_TEXT_FILE_TYPE      1u
#define MOLLY_ASSET_IMAGE_TYPE          2u
#define MOLLY_ASSET_TEXTURE_TYPE        3u
#define MOLLY_ASSET_MATERIAL_TYPE       4u
#define MOLLY_ASSET_STATIC_MESH_TYPE    5u
#define MOLLY_ASSET_MODEL_TYPE          6u
#define MOLLY_ASSET_UNKOWN_TYPE         0u

/*
    Asset Registry Handle (64bits)
    type     generation    index
    [8 bits ][   24 bits ][     32 bits     ]
*/
struct ARegHandle {
    u64 value;
    u32 type() {
        return static_cast<u32>(value >> 56) & 0xFF; 
    }
    u32 generation() {
        return static_cast<u32>(value >> 32) & 0xFFFFFFu;
    }
    u32 index() {
        return static_cast<u32>(value & 0xFFFFFFFFu);
    }
    static ARegHandle make(u32 type, u32 generation, u32 index) {
        u64 val = ((static_cast<u64>(type))       << 56) |
                  ((static_cast<u64>(generation)) << 32) |
                  (static_cast<u64>(index));
                    
        return {val};
    }
};

#endif  // MOLLY_HANDLE_H
#pragma once

#include "common/common.hpp"

namespace Hydra::HW::TegraX1::GPU {

struct NvSurface {
    u32 width;
    u32 height;
    /*ColorFormat*/ u32 color_format;
    /*Layout*/ u32 layout;
    u32 pitch;
    u32 unused; // usually this field contains the nvmap handle, but it's
                // completely unused/overwritten during marshalling
    u32 offset;
    /*Kind*/ u32 kind;
    u32 block_height_log2;
    /*DisplayScanFormat*/ u32 scan;
    u32 second_field_offset;
    u64 flags;
    u64 size;
    u32 unk[6]; // compression related
};

struct NvGraphicsBuffer {
    i32 unk0;       // -1
    i32 nvmap_id;   // nvmap object id
    u32 unk2;       // 0
    u32 magic;      // 0xDAFFCAFF
    u32 pid;        // 42
    u32 type;       // ?
    u32 usage;      // GRALLOC_USAGE_* bitmask
    u32 format;     // PIXEL_FORMAT_*
    u32 ext_format; // copy of the above (in most cases)
    u32 stride;     // in pixels!
    u32 total_size; // in bytes
    u32 num_planes; // usually 1
    u32 unk12;      // 0
    NvSurface planes[3];
    u64 unused; // official sw writes a pointer to bookkeeping data here, but
                // it's otherwise completely unused/overwritten during
                // marshalling
};

struct TextureDescriptor {
    u32 width;
    u32 height;
    // TODO: more

    TextureDescriptor(const NvGraphicsBuffer& buff) {
        // TODO: why are there more planes?
        width = buff.planes[0].width;
        height = buff.planes[0].height;
    }
};

} // namespace Hydra::HW::TegraX1::GPU

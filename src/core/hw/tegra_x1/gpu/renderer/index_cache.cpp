#include "core/hw/tegra_x1/gpu/renderer/index_cache.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"
#include "core/hw/tegra_x1/gpu/renderer/buffer_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

namespace {

inline usize get_index_count_quads_to_triangles(usize count) {
    return count / 4 * 6;
}

inline Engines::PrimitiveType get_primitive_type_quads_to_triangles() {
    return Engines::PrimitiveType::Triangles;
}

inline usize get_index_count_triangle_fan_to_triangle_strip(usize count) {
    return count;
}

inline Engines::PrimitiveType
get_primitive_type_triangle_fan_to_triangle_strip() {
    return Engines::PrimitiveType::TriangleStrip;
}

#define GET_INDEX(idx, index)                                                  \
    switch (index_type) {                                                      \
    case Engines::IndexType::UInt8:                                            \
        idx = GET_INDEX_IMPL(u8, index);                                       \
        break;                                                                 \
    case Engines::IndexType::UInt16:                                           \
        idx = GET_INDEX_IMPL(u16, index);                                      \
        break;                                                                 \
    case Engines::IndexType::UInt32:                                           \
        idx = GET_INDEX_IMPL(u32, index);                                      \
        break;                                                                 \
    default:                                                                   \
        break;                                                                 \
    }

#define ADD_INDEX(index)                                                       \
    switch (index_type) {                                                      \
    case Engines::IndexType::UInt8:                                            \
        /* TODO: check for u8 support */                                       \
        *reinterpret_cast<u16*>(out) = index;                                  \
        out += sizeof(u16);                                                    \
        break;                                                                 \
    case Engines::IndexType::UInt16:                                           \
        *reinterpret_cast<u16*>(out) = index;                                  \
        out += sizeof(u16);                                                    \
        break;                                                                 \
    case Engines::IndexType::UInt32:                                           \
        *reinterpret_cast<u32*>(out) = index;                                  \
        out += sizeof(u32);                                                    \
        break;                                                                 \
    default:                                                                   \
        break;                                                                 \
    }

#define ADD_INDEX_AUTO(index)                                                  \
    {                                                                          \
        u32 idx;                                                               \
        GET_INDEX(idx, index);                                                 \
        ADD_INDEX(index);                                                      \
    }

#define DEFINE_DECODER_PROTOTYPE(name)                                         \
    void decode_##name##_auto(uptr in_unused, uptr out,                        \
                              Engines::IndexType index_type, usize count)
#define GET_INDEX_IMPL(type, index) (index)

#include "core/hw/tegra_x1/gpu/renderer/index_decoders.inc"

#undef GET_INDEX_IMPL
#undef DEFINE_DECODER_PROTOTYPE

#define DEFINE_DECODER_PROTOTYPE(name)                                         \
    void decode_##name(uptr in, uptr out, Engines::IndexType index_type,       \
                       usize count)
#define GET_INDEX_IMPL(type, index) reinterpret_cast<type*>(in)[index]

#include "core/hw/tegra_x1/gpu/renderer/index_decoders.inc"

#undef GET_INDEX_IMPL
#undef DEFINE_DECODER_PROTOTYPE

} // namespace

IndexCache::~IndexCache() {
    for (auto& [key, index_buffer] : cache)
        RENDERER->FreeTemporaryBuffer(index_buffer);
}

BufferBase* IndexCache::Decode(const IndexDescriptor& descriptor,
                               Engines::IndexType& out_type,
                               Engines::PrimitiveType& out_primitive_type,
                               u32& out_count) {
#define PRIMITIVE_TYPE_SWITCH(macro)                                           \
    switch (descriptor.primitive_type) {                                       \
    case Engines::PrimitiveType::Quads:                                        \
        /* TODO: check for quads support */                                    \
        macro(quads_to_triangles);                                             \
        break;                                                                 \
    case Engines::PrimitiveType::TriangleFan:                                  \
        /* TODO: check for triangle fan support */                             \
        macro(triangle_fan_to_triangle_strip);                                 \
        break;                                                                 \
    default:                                                                   \
        return descriptor.src_index_buffer;                                    \
    }

    out_type = descriptor.type;
    out_primitive_type = descriptor.primitive_type;
    out_count = descriptor.count;

#define GET_PARAMS(name)                                                       \
    out_primitive_type = get_primitive_type_##name();                          \
    out_count = get_index_count_##name(descriptor.count)

    // Returns src index buffer if no decoding is needed
    PRIMITIVE_TYPE_SWITCH(GET_PARAMS)

    switch (out_count) {
    case 0 ... 0xff:
        // TODO: check for u8 support
        out_type = Engines::IndexType::UInt16;
        break;
    case 0x100 ... 0xffff:
        out_type = Engines::IndexType::UInt16;
        break;
    case 0x10000 ... 0xffffffff:
        out_type = Engines::IndexType::UInt32;
        break;
    }

    u64 hash = Hash(descriptor);
    auto& index_buffer = cache[hash];
    if (index_buffer)
        return index_buffer;

    usize index_size = get_index_type_size(out_type);
    index_buffer = RENDERER->AllocateTemporaryBuffer(out_count * index_size);
    uptr in_ptr = 0;
    if (descriptor.src_index_buffer)
        in_ptr = descriptor.src_index_buffer->GetDescriptor().ptr;
    auto out_ptr = index_buffer->GetDescriptor().ptr;

#define DECODE(name) decode_##name(in_ptr, out_ptr, out_type, descriptor.count)

#define DECODE_MACRO_AUTO(name) DECODE(name##_auto);
#define DECODE_MACRO(name) DECODE(name);

    if (descriptor.src_index_buffer == nullptr) {
        PRIMITIVE_TYPE_SWITCH(DECODE_MACRO_AUTO)
    } else {
        PRIMITIVE_TYPE_SWITCH(DECODE_MACRO)
    }

    return index_buffer;
} // namespace Hydra::HW::TegraX1::GPU::Renderer

u64 IndexCache::Hash(const IndexDescriptor& descriptor) {
    u64 hash = 0;
    hash += (u64)descriptor.type;
    hash = std::rotl(hash, 3);
    hash += (u64)descriptor.primitive_type;
    hash = std::rotl(hash, 5);
    hash += (u64)descriptor.src_index_buffer; // TODO: don't hash it like this
    hash = std::rotl(hash, 47);
    hash += (u64)descriptor.count;
    hash = std::rotl(hash, 17);

    return hash;
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer

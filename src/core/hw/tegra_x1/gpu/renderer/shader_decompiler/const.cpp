#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler {

DataType to_data_type(Engines::VertexAttribType vertex_attrib_type) {
    switch (vertex_attrib_type) {
    case Engines::VertexAttribType::Snorm:
        return DataType::Float;
    case Engines::VertexAttribType::Unorm:
        return DataType::Float;
    case Engines::VertexAttribType::Sint:
        return DataType::Int;
    case Engines::VertexAttribType::Uint:
        return DataType::UInt;
    case Engines::VertexAttribType::Sscaled:
        return DataType::Int; // TODO: use float if the Rendered backend
                              // supports scaled attributes
    case Engines::VertexAttribType::Uscaled:
        return DataType::UInt; // TODO: use float if the Rendered backend
                               // supports scaled attributes
    case Engines::VertexAttribType::Float:
        return DataType::Float;
    default:
        LOG_WARNING(ShaderDecompiler, "Unknown vertex attrib type {}",
                    vertex_attrib_type);
        return DataType::None;
    }
}

u32 get_load_store_count(LoadStoreMode mode) {
    switch (mode) {
    case LoadStoreMode::B32:
        return 1;
    case LoadStoreMode::B64:
        return 2;
    case LoadStoreMode::B96:
        return 3;
    case LoadStoreMode::B128:
        return 4;
    default:
        LOG_ERROR(ShaderDecompiler, "Unknown load store mode {}", mode);
        return 0;
    }
}

const SV get_sv_from_addr(u64 addr) {
    ASSERT_ALIGNMENT_DEBUG(addr, 4, ShaderDecompiler, "Address");

    struct SVBase {
        SVSemantic semantic;
        u64 base_addr;
    };

    static constexpr SVBase bases[] = {
        {SVSemantic::UserInOut, 0x80},
        {SVSemantic::Position, 0x70},
    };

    for (const auto& base : bases) {
        if (addr >= base.base_addr) {
            return SV(base.semantic,
                      static_cast<u8>((addr - base.base_addr) >> 4),
                      static_cast<u8>((addr >> 2) & 0x3));
        }
    }

    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "SV address 0x{:02x}", addr);

    return SVSemantic::Invalid;
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler

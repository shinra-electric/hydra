#pragma once

#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class BufferBase;

struct IndexDescriptor {
    Engines::IndexType type;
    Engines::PrimitiveType primitive_type;
    usize count;
    BufferBase* src_index_buffer;
};

class IndexCache {
  public:
    ~IndexCache();

    BufferBase* Decode(const IndexDescriptor& descriptor,
                       Engines::IndexType& out_type,
                       Engines::PrimitiveType& out_primitive_type,
                       u32& out_count);
    u64 Hash(const IndexDescriptor& descriptor);

  private:
    std::map<u64, BufferBase*> cache;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer

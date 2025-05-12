#pragma once

#include "common/types.hpp"

namespace hydra {

void decompress_lz4(const u8* src, usize src_size, u8* dst, usize dst_size);

} // namespace hydra

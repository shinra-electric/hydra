#pragma once

#include "core/horizon/loader/loader_base.hpp"

namespace hydra::horizon::loader {

struct Segment {
    u32 file_offset;
    u32 memory_offset;
    u32 size;
};

class NsoLoader : public LoaderBase {
  public:
    NsoLoader(StreamReader reader, const std::string_view name_ = "main",
              const bool is_entry_point_ = true);

    std::optional<kernel::ProcessParams>
    LoadProcess(StreamReader reader,
                const std::string_view rom_filename) override;

  private:
    std::string name;
    const bool is_entry_point;

    u32 text_offset;
    usize executable_size{0};
    struct {
        Segment seg;
        usize file_size;
        bool compressed;
    } segments[3];
    u32 dyn_str_offset;
    u32 dyn_str_size;
    u32 dyn_sym_offset;
    u32 dyn_sym_size;
};

} // namespace hydra::horizon::loader

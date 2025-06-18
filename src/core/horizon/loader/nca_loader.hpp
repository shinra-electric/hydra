#pragma once

#include "core/horizon/filesystem/content_archive.hpp"
#include "core/horizon/loader/loader_base.hpp"

namespace hydra::horizon::loader {

class NcaLoader : public LoaderBase {
  public:
    NcaLoader(filesystem::FileBase* file) : content_archive(file) {}

    std::optional<kernel::ProcessParams> LoadProcess() override;
    void LoadNintendoLogo(uchar4*& out_data, usize& out_width,
                          usize& out_height) override;
    void LoadStartupMovie(uchar4*& out_data, usize& out_width,
                          usize& out_height, u32& out_frame_count) override;

  private:
    filesystem::ContentArchive content_archive;

    // Helpers
    std::optional<kernel::ProcessParams> LoadCode(filesystem::Directory* dir);
};

} // namespace hydra::horizon::loader

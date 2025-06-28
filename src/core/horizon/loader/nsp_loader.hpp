#pragma once

#include "core/horizon/filesystem/partition_filesystem.hpp"
#include "core/horizon/loader/nca_loader.hpp"

namespace hydra::horizon::loader {

class NspLoader : public LoaderBase {
  public:
    NspLoader(filesystem::FileBase* file)
        : NspLoader(std::move(
              *filesystem::PartitionFilesystem().Initialize<false>(file))) {}
    NspLoader(const filesystem::PartitionFilesystem& pfs_);

    u64 GetTitleID() const override { return program_nca_loader->GetTitleID(); }

    std::optional<kernel::ProcessParams> LoadProcess() override {
        return program_nca_loader->LoadProcess();
    }
    void LoadNintendoLogo(uchar4*& out_data, usize& out_width,
                          usize& out_height) override {
        program_nca_loader->LoadNintendoLogo(out_data, out_width, out_height);
    }
    void LoadStartupMovie(uchar4*& out_data,
                          std::vector<std::chrono::milliseconds>& out_delays,
                          usize& out_width, usize& out_height,
                          u32& out_frame_count) override {
        program_nca_loader->LoadStartupMovie(out_data, out_delays, out_width,
                                             out_height, out_frame_count);
    }

  private:
    filesystem::PartitionFilesystem pfs;

    NcaLoader* program_nca_loader{nullptr};
};

} // namespace hydra::horizon::loader

#pragma once

#include "core/horizon/filesystem/file.hpp"

namespace hydra::horizon::filesystem {

struct SparseFileEntry {
    u64 offset;
    IFile* file;
};

class SparseFile : public IFile {
  public:
    SparseFile(std::span<SparseFileEntry> entries_, usize size_) : size{size_} {
        // Sort the entries by offset
        entries.assign(entries_.begin(), entries_.end());
        std::sort(entries.begin(), entries.end(),
                  [](const SparseFileEntry& a, const SparseFileEntry& b) {
                      return a.offset < b.offset;
                  });
    }

    void Resize(usize new_size) override {
        LOG_FATAL(Filesystem, "Sparse file cannot be resized (size: 0x{:x})",
                  new_size);
    }

    io::IStream* Open(FileOpenFlags flags) override {
        std::vector<io::SparseStreamEntry> streams;
        streams.reserve(entries.size());
        for (const auto& entry : entries) {
            streams.push_back(
                {range(entry.offset, entry.offset + entry.file->GetSize()),
                 entry.file->Open(flags)});
        }

        return new io::OwnedSparseStream(std::move(streams), size);
    }

    usize GetSize() override { return size; }

  private:
    std::vector<SparseFileEntry> entries;
    usize size;

    void DeleteImpl() override {
        LOG_FATAL(Filesystem, "Cannot delete sparse file");
    }
};

} // namespace hydra::horizon::filesystem

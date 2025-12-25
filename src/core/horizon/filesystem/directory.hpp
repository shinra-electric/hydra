#pragma once

#include "core/horizon/filesystem/entry_base.hpp"

namespace hydra::horizon::filesystem {

class FileBase;

class Directory : public EntryBase {
  public:
    Directory() = default;
    Directory(const std::string_view host_path);
    ~Directory() override;

    bool IsDirectory() const override { return true; }

    [[nodiscard]] FsResult Delete(bool recursive = false) override;

    [[nodiscard]] FsResult AddEntry(const std::string_view path,
                                    EntryBase* entry,
                                    bool add_intermediate = false);
    [[nodiscard]] FsResult AddEntry(const std::string_view path,
                                    const std::string_view host_path,
                                    bool add_intermediate = false);

    [[nodiscard]] FsResult DeleteEntry(const std::string_view path,
                                       bool recursive = false);

    [[nodiscard]] FsResult GetEntry(const std::string_view path,
                                    EntryBase*& out_entry);
    [[nodiscard]] FsResult GetFile(const std::string_view path,
                                   FileBase*& out_file);
    [[nodiscard]] FsResult GetDirectory(const std::string_view path,
                                        Directory*& out_directory);

    // TODO: find a better way
    [[nodiscard]] FsResult GetEntry(const std::string_view path,
                                    const EntryBase*& out_entry) const {
        return const_cast<Directory*>(this)->GetEntry(path, out_entry);
    }
    [[nodiscard]] FsResult GetFile(const std::string_view path,
                                   FileBase*& out_file) const {
        return const_cast<Directory*>(this)->GetFile(path, out_file);
    }
    [[nodiscard]] FsResult GetDirectory(const std::string_view path,
                                        Directory*& out_directory) const {
        return const_cast<Directory*>(this)->GetDirectory(path, out_directory);
    }

    // Getters
    const std::map<std::string, EntryBase*>& GetEntries() const {
        return entries;
    }

  protected:
    std::map<std::string, EntryBase*> entries;

  private:
    // Impl
    FsResult AddEntryImpl(const std::span<std::string_view> path,
                          EntryBase* entry, bool add_intermediate = false);
    FsResult DeleteEntryImpl(const std::span<std::string_view> path,
                             bool recursive = false);
    FsResult GetEntryImpl(const std::span<std::string_view> path,
                          EntryBase*& out_entry);

    // Helpers
    void BreakPath(std::string_view path,
                   std::vector<std::string_view>& out_path);
};

} // namespace hydra::horizon::filesystem

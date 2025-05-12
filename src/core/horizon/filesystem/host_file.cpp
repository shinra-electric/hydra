#include "core/horizon/filesystem/host_file.hpp"

namespace hydra::horizon::filesystem {

HostFile::HostFile(const std::string& host_path_, u64 offset, usize size_limit_)
    : FileBase(offset), host_path{host_path_}, size_limit{size_limit_} {
    if (!std::filesystem::exists(host_path)) {
        // Intermediate directories
        std::filesystem::create_directories(
            std::filesystem::path(host_path).parent_path());

        // Empty file
        // TODO: is there a better way to create an empty file?
        std::ofstream ofs(host_path);
        ofs.close();
    }
}

void HostFile::Resize(usize new_size) {
    std::filesystem::resize_file(host_path, new_size);
}

FileStream HostFile::Open(FileOpenFlags flags) {
    std::ios::openmode std_flags = std::ios::binary;
    if (any(flags & FileOpenFlags::Read))
        std_flags |= std::ios::in;
    if (any(flags & FileOpenFlags::Write))
        std_flags |= std::ios::out;
    if (any(flags & FileOpenFlags::Append))
        std_flags |= std::ios::app;
    auto stream = new std::fstream(host_path, std_flags);

    return FileStream(stream, offset, GetSize(), flags);
}

void HostFile::Close(FileStream& stream) {
    auto fs = dynamic_cast<std::fstream*>(stream.GetStream());
    ASSERT(fs, Filesystem, "Invalid stream type");
    fs->close();
    delete fs;
}

usize HostFile::GetSize() {
    usize file_size = std::filesystem::file_size(host_path);
    return std::min(static_cast<usize>(file_size) - offset, size_limit);
}

} // namespace hydra::horizon::filesystem

#include "core/horizon/filesystem/host_file.hpp"

#define LOG_FS_ACCESS(host_path, f, ...)                                       \
    if (CONFIG_INSTANCE.GetLogFsAccess()) {                                    \
        LOG_INFO(Filesystem, "\"{}\": " f,                                     \
                 host_path PASS_VA_ARGS(__VA_ARGS__));                         \
    }

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

        LOG_FS_ACCESS(host_path, "file created");
    }

    size = std::filesystem::file_size(host_path);
}

HostFile::~HostFile() {
    // Resize the file to the requested size
    if (std::filesystem::exists(host_path)) {
        if (std::filesystem::file_size(host_path) != size)
            std::filesystem::resize_file(host_path, size);
    }
}

void HostFile::Resize(usize new_size) {
    size = new_size;

    LOG_FS_ACCESS(host_path, "file resized (size: {})", new_size);
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

    LOG_FS_ACCESS(host_path, "file opened");

    return FileStream(stream, offset, GetSize(), flags);
}

void HostFile::Close(FileStream& stream) {
    auto fs = dynamic_cast<std::fstream*>(stream.GetStream());
    ASSERT(fs, Filesystem, "Invalid stream type");
    fs->close();
    delete fs;

    LOG_FS_ACCESS(host_path, "file closed");
}

usize HostFile::GetSize() {
    return std::min(static_cast<usize>(size) - offset, size_limit);
}

void HostFile::DeleteImpl() {
    std::filesystem::remove(host_path);

    LOG_FS_ACCESS(host_path, "file deleted");
}

} // namespace hydra::horizon::filesystem

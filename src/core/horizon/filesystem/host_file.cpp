#include "core/horizon/filesystem/host_file.hpp"

namespace Hydra::Horizon::Filesystem {

HostFile::HostFile(const std::string& host_path_, u64 offset, usize size_limit_)
    : FileBase(offset), host_path{host_path_}, size_limit{size_limit_} {}

HostFile::~HostFile() {
    if (stream) {
        LOG_WARNING(HorizonFilesystem, "File not closed properly");
        delete stream;
    }
}

usize HostFile::GetSize() {
    ASSERT_DEBUG(stream, HorizonFilesystem, "File is not open");
    stream->seekg(0, std::ios::end);
    auto size = stream->tellg();

    return std::min(static_cast<usize>(size) - offset, size_limit);
}

void HostFile::OpenImpl() {
    ASSERT_DEBUG(!stream, HorizonFilesystem, "File is already open");
    stream = new std::fstream(host_path, std::ios::in | std::ios::binary);
}

void HostFile::CloseImpl() {
    ASSERT_DEBUG(stream, HorizonFilesystem, "File is not open");
    stream->close();
    delete stream;
    stream = nullptr;
}

} // namespace Hydra::Horizon::Filesystem

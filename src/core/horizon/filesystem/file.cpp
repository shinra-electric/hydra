#include "core/horizon/filesystem/file.hpp"

namespace Hydra::Horizon::Filesystem {

File::File(const std::string& host_path_, u64 offset_, usize size_)
    : host_path{host_path_}, offset{offset_}, size{size_} {
    if (size == invalid<usize>()) {
        std::ifstream file(host_path, std::ios::ate | std::ios::binary);
        size = file.tellg();
        file.close();
    }
}

File::~File() {
    if (stream) {
        LOG_WARNING(HorizonFilesystem, "File not closed properly");
        Close();
    }
}

void File::Open() {
    ASSERT_DEBUG(!stream, HorizonFilesystem, "File is already open");
    stream = new std::ifstream(host_path, std::ios::in | std::ios::binary);
}

void File::Close() {
    ASSERT_DEBUG(stream, HorizonFilesystem, "File not open");
    stream->close();
    delete stream;
    stream = nullptr;
}

} // namespace Hydra::Horizon::Filesystem

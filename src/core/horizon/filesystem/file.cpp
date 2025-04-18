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
    if (!stream)
        stream = new std::ifstream(host_path, std::ios::in | std::ios::binary);

    refs++;
}

void File::Close() {
    ASSERT(refs != 0, HorizonFilesystem, "Cannot close file that is not open");
    refs--;

    if (refs == 0) {
        stream->close();
        delete stream;
        stream = nullptr;
    }
}

} // namespace Hydra::Horizon::Filesystem

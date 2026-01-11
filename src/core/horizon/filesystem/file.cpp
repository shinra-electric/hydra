#include "core/horizon/filesystem/file.hpp"

namespace hydra::horizon::filesystem {

void IFile::Save(std::string_view host_path) const {
    // Create file
    std::fstream ofs(
        std::string(host_path),
        std::ios::out | std::ios::binary); // HACK: construct a temporary string
    io::IostreamStream out_stream(ofs);

    // Read
    const auto stream =
        const_cast<IFile*>(this)->Open(FileOpenFlags::Read); // HACK
    std::array<u8, 0x800> buffer;
    while (stream->GetSeek() < stream->GetSize()) {
        std::span<u8> span(buffer.data(),
                           buffer.data() +
                               std::min(static_cast<u64>(buffer.size()),
                                        stream->GetSize() - stream->GetSeek()));
        stream->ReadToSpan(span);
        out_stream.WriteSpan(std::span<const u8>(span));
    }
}

} // namespace hydra::horizon::filesystem

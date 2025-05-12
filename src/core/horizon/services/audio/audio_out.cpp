#include "core/horizon/services/audio/audio_out.hpp"

namespace hydra::horizon::services::audio {

DEFINE_SERVICE_COMMAND_TABLE(IAudioOut, 1, Start, 3, AppendAudioOutBuffer, 4,
                             RegisterBufferEvent, 5, GetReleasedAudioOutBuffers)

IAudioOut::IAudioOut() : buffer_event(new kernel::Event()) {}

result_t
IAudioOut::AppendAudioOutBuffer(u64 buffer_client_ptr,
                                InBuffer<BufferAttr::MapAlias> buffer_buffer) {
    const auto buffer = buffer_buffer.reader->Read<AudioOutBuffer>();
    // TODO: start playback

    buffers.emplace_back(buffer, buffer_client_ptr);
    return RESULT_SUCCESS;
}

result_t
IAudioOut::RegisterBufferEvent(OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = buffer_event.id;
    return RESULT_SUCCESS;
}

result_t IAudioOut::GetReleasedAudioOutBuffers(
    u32* out_count, OutBuffer<BufferAttr::MapAlias> out_buffers_buffer) {
    return GetReleasedAudioOutBuffersImpl(out_count,
                                          *out_buffers_buffer.writer);
}

result_t IAudioOut::GetReleasedAudioOutBuffersImpl(u32* out_count,
                                                   Writer& out_buffers_writer) {
    // HACK: pretend as if though all the buffers finished playing
    *out_count = buffers.size();
    for (const auto& [buffer, client_ptr] : buffers)
        out_buffers_writer.Write(client_ptr);
    buffers.clear();

    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::audio

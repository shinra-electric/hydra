#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::audio {

struct AudioOutBuffer {
    u64 next_ptr; // Unused
    u64 sample_buffer_ptr;
    u64 sample_buffer_capacity;
    u64 sample_buffer_data_size;
    u64 sample_buffer_data_offset; // TODO: unused/ignored?
};

class IAudioOut : public ServiceBase {
  public:
    IAudioOut();

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    kernel::HandleWithId<kernel::Event> buffer_event;
    std::vector<std::pair<AudioOutBuffer, u64>> buffers;

    // Commands
    STUB_REQUEST_COMMAND(Start);
    STUB_REQUEST_COMMAND(Stop);
    result_t AppendAudioOutBuffer(u64 buffer_client_ptr,
                                  InBuffer<BufferAttr::MapAlias> buffer_buffer);
    result_t RegisterBufferEvent(OutHandle<HandleAttr::Copy> out_handle);
    result_t GetReleasedAudioOutBuffers(
        u32* out_count, OutBuffer<BufferAttr::MapAlias> out_buffers_buffer);

    result_t GetReleasedAudioOutBuffersImpl(u32* out_count,
                                            Writer& out_buffers_writer);
};

} // namespace hydra::horizon::services::audio

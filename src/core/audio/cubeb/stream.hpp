#pragma once

#include "core/audio/stream_base.hpp"

namespace hydra::audio {

class Stream final : public StreamBase {
  public:
    Stream(buffer_finished_callback_fn_t buffer_finished_callback);
    ~Stream() override;

    buffer_id_t EnqueueBuffer(sized_ptr buffer) override;
};

} // namespace hydra::audio

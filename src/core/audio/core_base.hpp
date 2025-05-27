#pragma once

#include "core/audio/stream_base.hpp"

namespace hydra::audio {

class CoreBase {
  public:
    virtual StreamBase*
    CreateStream(buffer_finished_callback_fn_t buffer_finished_callback) = 0;
};

} // namespace hydra::audio

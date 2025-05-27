#pragma once

#include "core/audio/const.hpp"

namespace hydra::audio {

using buffer_id_t = u64;

typedef std::function<void(buffer_id_t)> buffer_finished_callback_fn_t;

class StreamBase {
  public:
    StreamBase(buffer_finished_callback_fn_t buffer_finished_callback_)
        : buffer_finished_callback{buffer_finished_callback_} {}
    virtual ~StreamBase() {}

    virtual buffer_id_t EnqueueBuffer(sized_ptr buffer) = 0;

  protected:
    buffer_finished_callback_fn_t buffer_finished_callback;
};

} // namespace hydra::audio

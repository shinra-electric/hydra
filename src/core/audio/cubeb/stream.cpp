#include "core/audio/cubeb/stream.hpp"

namespace hydra::audio {

Stream::Stream(buffer_finished_callback_fn_t buffer_finished_callback)
    : StreamBase(buffer_finished_callback) {
    // TODO
}

Stream::~Stream() {
    // TODO
}

buffer_id_t Stream::EnqueueBuffer(sized_ptr buffer) {
    // TODO
    return (buffer_id_t)buffer.GetPtr();
}

} // namespace hydra::audio

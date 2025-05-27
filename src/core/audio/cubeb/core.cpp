#include "core/audio/cubeb/core.hpp"

#include "core/audio/cubeb/stream.hpp"

namespace hydra::audio::cubeb {

Core::Core() {
    // TODO: initialize cubeb
}

StreamBase*
Core::CreateStream(buffer_finished_callback_fn_t buffer_finished_callback) {
    return new Stream(buffer_finished_callback);
}

} // namespace hydra::audio::cubeb

#pragma once

#include "core/audio/core_base.hpp"

namespace hydra::audio::cubeb {

class Core final : public CoreBase {
  public:
    Core();

    StreamBase* CreateStream(
        buffer_finished_callback_fn_t buffer_finished_callback) override;
};

} // namespace hydra::audio::cubeb
